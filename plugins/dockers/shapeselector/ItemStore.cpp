/*
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "ItemStore.h"

#include "FolderShape.h"
#include "TemplateShape.h"
#include "GroupShape.h"
#include "FolderBorder.h"

#include <KoShapeManager.h>
#include <KoProperties.h>
#include <KoShapeRegistry.h>
#include <KoGlobal.h>
#include <KLocale>
#include <KConfigGroup>
#include <KGlobal>

class ItemStorePrivate {
public:
    ItemStorePrivate() : mainFolder(0) { }

    void addFolder(FolderShape *folder)
    {
        Q_ASSERT(folder);
        if (folders.contains(folder))
            return;
        mainFolder = folder;
        if (!folders.isEmpty()) {
            folder->setBorder(folders[0]->border());
            int zIndex = 0;
            foreach (FolderShape *fs, folders)
                zIndex = qMax(zIndex, fs->zIndex());
            folder->setZIndex(zIndex+1);
        }
        folders.append(folder);
        if (folders.count() > 1)
            mainFolder = 0;
        foreach(KoShapeManager *sm, shapeManagers)
            sm->add(folder);
    }

    void removeFolder(FolderShape *folder)
    {
        Q_ASSERT(folder);
        Q_ASSERT(folder != mainFolder); // can't remove the last folder
        foreach(KoShapeManager *sm, shapeManagers)
            sm->remove(folder);
        folders.removeAll(folder);
        if (folders.count() == 1)
            mainFolder = folders[0];
    }

    void addShape(KoShape *shape)
    {
        foreach(KoShapeManager *sm, shapeManagers)
            sm->add(shape);
        shapes.append(shape);
    }

    void removeShape(KoShape *shape)
    {
        foreach(KoShapeManager *sm, shapeManagers)
            sm->remove(shape);
        shapes.removeAll(shape);
    }

    void addUser(KoShapeManager *sm)
    {
        shapeManagers.append(sm);
    }

    void removeUser(KoShapeManager *sm)
    {
        shapeManagers.removeAll(sm);
        KConfigGroup conf = KoGlobal::kofficeConfig()->group("ShapeSelectorPlugin");
        const int previouslyConfiguredBooks = conf.readEntry("books", 0);
        conf.writeEntry("books", folders.size());
        int index = 1;
        foreach(FolderShape *folder, folders) {
            conf.writeEntry(QString::fromLatin1("book.%1_name").arg(index).toAscii().data(), folder->name());
            if (folder == mainFolder)
                continue;
            conf.writeEntry(QString::fromLatin1("book.%1_position").arg(index).toAscii().data(), folder->position());
            conf.writeEntry(QString::fromLatin1("book.%1_size").arg(index).toAscii().data(), folder->size());
            conf.writeEntry(QString::fromLatin1("book.%1_items").arg(index).toAscii().data(), folder->save().toByteArray());
            ++index;
        }
        for (int i=folders.size()+1; i <= previouslyConfiguredBooks; i++) { // remove surplus
            conf.deleteEntry(QString::fromLatin1("book.%1_name").arg(i).toAscii().data());
            conf.deleteEntry(QString::fromLatin1("book.%1_position").arg(i).toAscii().data());
            conf.deleteEntry(QString::fromLatin1("book.%1_size").arg(i).toAscii().data());
            conf.deleteEntry(QString::fromLatin1("book.%1_items").arg(i).toAscii().data());
        }

        if (shapeManagers.count() == 0) // last one
        {
            qDeleteAll(folders);
            folders.clear();
            qDeleteAll(shapes);
            shapes.clear();
        }
    }

    QList<KoShape*> shapes;
    QList<FolderShape *> folders;
    QList<KoShapeManager*> shapeManagers;
    FolderShape *mainFolder;
};

Q_GLOBAL_STATIC(ItemStorePrivate, s_itemStorePrivate)


ItemStore::ItemStore(KoShapeManager *shapeManager)
    : m_shapeManager(shapeManager)
{
    s_itemStorePrivate()->addUser(shapeManager);
    if (s_itemStorePrivate()->shapeManagers.count() > 1) {
        KoShapeManager *other = s_itemStorePrivate()->shapeManagers.first();
        m_shapeManager->setShapes(other->shapes());
    }
}

ItemStore::~ItemStore()
{
    s_itemStorePrivate()->removeUser(m_shapeManager);
}

void ItemStore::addFolder(FolderShape *folder)
{
    s_itemStorePrivate()->addFolder(folder);
}

void ItemStore::removeFolder(FolderShape *folder)
{
    s_itemStorePrivate()->removeFolder(folder);
}

QList<FolderShape*> ItemStore::folders() const
{
    return s_itemStorePrivate()->folders;
}

void ItemStore::addShape(KoShape *shape)
{
    s_itemStorePrivate()->addShape(shape);
}

void ItemStore::removeShape(KoShape *shape)
{
    s_itemStorePrivate()->removeShape(shape);
}

QList<KoShape*> ItemStore::shapes() const
{
    return s_itemStorePrivate()->shapes;
}

FolderShape * ItemStore::mainFolder() const
{
    return s_itemStorePrivate()->mainFolder;
}

QRectF ItemStore::loadShapeTypes()
{
    qreal maxHeight = 0;

    if (s_itemStorePrivate()->shapeManagers.count() > 1)
        return QRectF(); // someone else already did the init.

    FolderShape *mainFolder = 0;

    KConfigGroup conf = KoGlobal::kofficeConfig()->group("ShapeSelectorPlugin");

    QRectF boundingRect;
    const int books = conf.readEntry("books", 0);
    for (int i=1; i <= books; i++) {
        FolderShape *folder = new FolderShape();
        folder->setName(
            conf.readEntry(QString::fromLatin1("book.%1_name").arg(i).toAscii().data(), "Unnamed"));
        folder->setPosition(
            conf.readEntry(QString::fromLatin1("book.%1_position").arg(i).toAscii().data(), QPointF()));
        folder->setSize(
            conf.readEntry(QString::fromLatin1("book.%1_size").arg(i).toAscii().data(), QSizeF(100, 100)));
        QByteArray children = conf.readEntry(QString::fromLatin1("book.%1_items").arg(i).toAscii().data(), QByteArray());
        if (children.size()) {
            QDomDocument doc;
            QString error;
            int line, column;
            if (doc.setContent(children, false, &error, &line, &column)) {
                folder->load(doc);
                foreach(KoShape *child, folder->iterator())
                    s_itemStorePrivate()->addShape(child);
            }
            else {
                kWarning() << "ERROR: Could not parse xml for folder" << i << "at Line" << line << "Column" << column;
                kWarning() << "  " << error;
            }
        }
        s_itemStorePrivate()->addFolder(folder);
        if (mainFolder == 0) {
            mainFolder = folder;
            if (books > 1)
                mainFolder->setBorder(new FolderBorder());
            boundingRect = mainFolder->boundingRect();
        }
        else
            boundingRect = boundingRect.unite(folder->boundingRect());
    }

    if (mainFolder == 0) {
        mainFolder = new FolderShape();
        mainFolder->setName(i18n("Templates"));
        boundingRect = mainFolder->boundingRect();
        s_itemStorePrivate()->addFolder(mainFolder);
    }

    foreach(QString id, KoShapeRegistry::instance()->keys()) {
        KoShapeFactory *factory = KoShapeRegistry::instance()->value(id);
        bool oneAdded=false;
        foreach(KoShapeTemplate shapeTemplate, factory->templates()) {
            oneAdded=true;
            TemplateShape *ts = new TemplateShape(shapeTemplate);
            KoShapeTemplate t1 = ts->shapeTemplate();
            foreach(KoShape *shape, s_itemStorePrivate()->shapes) {
                TemplateShape *t = dynamic_cast<TemplateShape*> (shape);
                if (t == 0)
                    continue;
                KoShapeTemplate t2 = t->shapeTemplate();
                if (t1.id == t2.id && t1.properties && t2.properties && t1.properties->operator==(*t2.properties)) { // already there
                    delete ts;
                    ts = 0;
                    break;
                }
            }
            if (ts) {
                s_itemStorePrivate()->addShape(ts);
                mainFolder->addChild(ts);
                maxHeight = qMax(maxHeight, ts->size().height());
            }
        }
        if(!oneAdded) {
            KoShape *group = new GroupShape(factory);
            mainFolder->addChild(group);
            s_itemStorePrivate()->addShape(group);
        }
    }

    return boundingRect;
}
