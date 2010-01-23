#include "parsedpresentation.h"
#include <QtCore/QBuffer>

using namespace PPT;

bool
readStream(POLE::Storage& storage, const char* streampath, QBuffer& buffer)
{
    POLE::Stream stream(&storage, streampath);
    QByteArray array;
    array.resize(stream.size());
    unsigned long r = stream.read((unsigned char*)array.data(), stream.size());
    if (r != stream.size()) {
        return false;
    }
    buffer.setData(array);
    buffer.open(QIODevice::ReadOnly);
    return true;
}
bool
parseCurrentUserStream(POLE::Storage& storage, CurrentUserStream& cus)
{
    QBuffer buffer;
    if (!readStream(storage, "/Current User", buffer)) {
        return false;
    }
    LEInputStream stream(&buffer);
    parseCurrentUserStream(stream, cus);
    if (stream.getPosition() != buffer.size()) {
        qDebug() << (buffer.size()-stream.getPosition())
                << "bytes left at the end of CurrentUserStream";
        return false;
    }
    return true;
}
bool
parsePowerPointStructs(POLE::Storage& storage, PowerPointStructs& pps)
{
    QBuffer buffer;
    if (!readStream(storage, "/PowerPoint Document", buffer)) {
        return false;
    }
    LEInputStream stream(&buffer);
    parsePowerPointStructs(stream, pps);
    if (stream.getPosition() != buffer.size()) {
        qDebug() << (buffer.size()-stream.getPosition())
                << "bytes left at the end of PowerPointStructs, so probably an error at position " << stream.getMaxPosition();
        return false;
    }
    return true;
}
/**
  * get the UserEditAtom that is at position @offset in the stream.
  * return 0 if it is not present.
  **/
template <typename T>
const T*
get(const PowerPointStructs& pps, quint32 offset)
{
    foreach(const PowerPointStruct& p, pps.anon) {
        if (p.anon.is<T>()
                && p.anon.get<T>()->streamOffset == offset) {
            return p.anon.get<T>();
        } else if (p.anon.is<MasterOrSlideContainer>()) {
            const MasterOrSlideContainer* m = p.anon.get<MasterOrSlideContainer>();
            if (m->anon.is<T>() && m->anon.get<T>()->streamOffset == offset) {
                return m->anon.get<T>();
            }
        }
    }
    return 0;
}
void
parsePersistDirectory(const PowerPointStructs& pps, const UserEditAtom* userEditAtom,
                      QMap<quint32, quint32>& persistDirectory)
{
    if (!userEditAtom) return;
    const PersistDirectoryAtom* persistDirectoryAtom
    = get<PersistDirectoryAtom>(pps, userEditAtom->offsetPersistDirectory);
    if (!persistDirectoryAtom) return;
    foreach(const PersistDirectoryEntry& pde, persistDirectoryAtom->rgPersistDirEntry) {
        for (int i = 0; i < pde.rgPersistOffset.size(); ++i) {
            if (!persistDirectory.contains(pde.persistId + i)) {
                persistDirectory[pde.persistId+i] = pde.rgPersistOffset[i];
            }
        }
    }
    quint32 offset = userEditAtom->offsetLastEdit;
    if (offset == 0) return;
    userEditAtom = get<UserEditAtom>(pps, offset);
    parsePersistDirectory(pps, userEditAtom, persistDirectory);
}

bool
ParsedPresentation::parse(POLE::Storage& storage)
{
    handoutMaster = 0;
    notesMaster = 0;

// read the CurrentUserStream and PowerPointStructs
    try {
        if (!parseCurrentUserStream(storage, currentUserStream)) {
            qDebug() << "error parsing CurrentUserStream";
            return false;
        }
        if (!parsePowerPointStructs(storage, presentation)) {
            qDebug() << "error parsing PowerPointStructs";
            return false;
        }
    } catch (...) {
        qDebug() << "caught unknown exception!";
        return false;
    }
    qDebug() << "parsing ok!";
// Part 1: Construct the persist object directory
    const UserEditAtom* userEditAtom = get<UserEditAtom>(presentation,
                                       currentUserStream.anon1.offsetToCurrentEdit);
    if (!userEditAtom) {
        qDebug() << "no userEditAtom";
        return false;
    }
    parsePersistDirectory(presentation, userEditAtom, persistDirectory);
// Part 2: Identify the document persist object
    if (persistDirectory.contains(userEditAtom->docPersistIdRef)) {
        documentContainer = get<DocumentContainer>(presentation,
                            persistDirectory[userEditAtom->docPersistIdRef]);
    }
    if (!documentContainer) {
        qDebug() << "no documentContainer";
        return false;
    }
// Part 3: identify the node master slide persist object
    quint32 persistId = documentContainer->documentAtom.notesMasterPersistIdRef;
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "no notesMaster";
            return false;
        }
        notesMaster = get<NotesContainer>(presentation, persistDirectory[persistId]);
        if (!notesMaster) {
            qDebug() << "no notesMaster";
            return false;
        }
    }
// Part 4: Identify the handout master slide persist object
    persistId = documentContainer->documentAtom.handoutMasterPersistIdRef;
    if (persistId) {
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "no handoutMaster";
            return false;
        }
        handoutMaster = get<HandoutContainer>(presentation, persistDirectory[persistId]);
        if (!handoutMaster) {
            qDebug() << "no handoutMaster";
            return false;
        }
    }
// Part 5: Identify the main master slide and title master slide persist objects
    int size = documentContainer->masterList.rgMasterPersistAtom.size();
    if (size == 0) {
        qDebug() << "no master slides";
        return false;
    }
    masters.resize(size);
    for (int i = 0; i < size;++i) {
        persistId = documentContainer->masterList.rgMasterPersistAtom[i].persistIdRef;
        if (!persistDirectory.contains(persistId)) {
            qDebug() << "cannot load master " << i;
            return false;
        }
        masters[i] = get<MasterOrSlideContainer>(presentation, persistDirectory[persistId]);
        if (!masters[i]) {
            qDebug() << "cannot load master " << i;
            return false;
        }
    }
// Part 6: Identify the presentation slide persist objects
    if (documentContainer->slideList) {
        size = documentContainer->slideList->rgChildRec.size();
        slides.resize(size);
        for (int i = 0; i < size;++i) {
            persistId = documentContainer->slideList->rgChildRec[i].slidePersistAtom.persistIdRef;
            if (!persistDirectory.contains(persistId)) {
                qDebug() << "cannot find persistId " << persistId << " for slide " << i;
                return false;
            }
            slides[i] = get<SlideContainer>(presentation, persistDirectory[persistId]);
            if (!slides[i]) {
                qDebug() << "cannot find slide " << i << " at offset "
                << persistDirectory[persistId];
                return false;
            }
        }
        qDebug() << "found " << slides.size() << " slides.";
    }
// Part 7: Identify the notes slide persist object
    if (documentContainer->notesList) {
        size = documentContainer->notesList->rgNotesPersistAtom.size();
        notes.resize(size);
        for (int i = 0; i < size;++i) {
            persistId = documentContainer->notesList->rgNotesPersistAtom[i].persistIdRef;
            if (!persistDirectory.contains(persistId)) {
                qDebug() << "cannot load notes " << i;
                return false;
            }
            notes[i] = get<NotesContainer>(presentation, persistDirectory[persistId]);
            if (!notes[i]) {
                qDebug() << "cannot load notes " << i;
                return false;
            }
        }
    }
// Part 8: Identify the ActiveX control persist objects
// Part 9: Identify the embedded OLE object persist objects
// Part 10: Identify the linked OLE object persist objects
// Part 11: Identify the VBA project persist object
    return true;
}
const PPT::MasterOrSlideContainer*
ParsedPresentation::getMaster(const SlideContainer* slide) const
{
    foreach(const MasterPersistAtom& m, documentContainer->masterList.rgMasterPersistAtom) {
        if (m.masterId == slide->slideAtom.masterIdRef) {
            quint32 offset = persistDirectory[m.persistIdRef];
            return get<MasterOrSlideContainer>(presentation, offset);
        }
    }
    return 0;
}