/* This file is part of the KDE project
   Copyright (C) 2006-2007 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbutils.h"

#include <QApplication>
#include <QFontMetrics>

#include <KMenu>
#include <KIconLoader>
#include <KIconEffect>

#include <kexidb/queryschema.h>
#include <kexidb/utils.h>
#include <formeditor/widgetlibrary.h>
#include <kexiutils/utils.h>
#include "../kexiformpart.h"
#include "../kexiformmanager.h"
#include <widget/utils/kexicontextmenuutils.h>

//! Static data for kexi forms
struct KexiFormStatics
{
    QPixmap dataSourceTagIcon() {
        initDataSourceTagIcon();
        return m_dataSourceTagIcon;
    }

    QPixmap dataSourceRTLTagIcon() {
        initDataSourceTagIcon();
        return m_dataSourceRTLTagIcon;
    }

    void initDataSourceTagIcon() {
        if (!m_dataSourceTagIcon.isNull())
            return;
        QFontMetrics fm(QApplication::fontMetrics());
        int size = KIconLoader::global()->currentSize(KIconLoader::Small);
        if (size < KIconLoader::SizeSmallMedium && fm.height() >= KIconLoader::SizeSmallMedium)
            size = KIconLoader::SizeSmallMedium;
        m_dataSourceTagIcon = KIconLoader::global()->loadIcon(QLatin1String("data-source-tag"), KIconLoader::Small, size);
        KIconEffect::semiTransparent(m_dataSourceTagIcon);
        m_dataSourceRTLTagIcon = QPixmap::fromImage(m_dataSourceTagIcon.toImage().mirrored(true /*h*/, false /*v*/));
    }
private:
    QPixmap m_dataSourceTagIcon;
    QPixmap m_dataSourceRTLTagIcon;
};

K_GLOBAL_STATIC(KexiFormStatics, g_KexiFormStatics)

//-------

QColor KexiFormUtils::lighterGrayBackgroundColor(const QPalette& palette)
{
    return KexiUtils::blendedColors(
        palette.color(QPalette::Active, QPalette::Background),
        palette.color(QPalette::Active, QPalette::Base),
        1, 2);
}

QPixmap KexiFormUtils::dataSourceTagIcon()
{
    return g_KexiFormStatics->dataSourceTagIcon();
}

QPixmap KexiFormUtils::dataSourceRTLTagIcon()
{
    return g_KexiFormStatics->dataSourceRTLTagIcon();
}

//-------

KexiDBWidgetContextMenuExtender::KexiDBWidgetContextMenuExtender(QObject* parent, KexiDataItemInterface* iface)
        : QObject(parent)
        , m_iface(iface)
        , m_contextMenuHasTitle(false)
{
}

KexiDBWidgetContextMenuExtender::~KexiDBWidgetContextMenuExtender()
{
}

void KexiDBWidgetContextMenuExtender::createTitle(QMenu *menu)
{
    if (!menu)
        return;
    m_contextMenu = menu;
    KMenu *kmenu = dynamic_cast<KMenu*>(menu);
#ifdef __GNUC__
#warning TODO KexiDBWidgetContextMenuExtender::createTitle() what to do to insert title into KMenu?
#else
#pragma WARNING( TODO KexiDBWidgetContextMenuExtender::createTitle() what to do to insert title into KMenu? )
#endif
    if (!kmenu)
        return;
    m_titleAction = kmenu->addTitle(QString(), m_contextMenu->actions().first());

    QString icon;
    if (dynamic_cast<QWidget*>(m_iface)) {
        icon = KexiFormManager::self()->library()->iconName(
                   dynamic_cast<QWidget*>(m_iface)->metaObject()->className());
    }
    m_contextMenuHasTitle = m_iface->columnInfo() ?
                            KexiContextMenuUtils::updateTitle(kmenu,
                                                              m_iface->columnInfo()->captionOrAliasOrName(),
                                                              KexiDB::simplifiedTypeName(*m_iface->columnInfo()->field), icon)
                            : false;

    if (!m_contextMenuHasTitle)
        kmenu->removeAction(m_titleAction);
    updatePopupMenuActions();
}

void KexiDBWidgetContextMenuExtender::updatePopupMenuActions()
{
    if (!m_contextMenu)
        return;
    const bool readOnly = m_iface->isReadOnly();

    foreach(QAction* action, m_contextMenu->actions()) {
        const QString text(action->text());
        if (text == QObject::tr("Cu&t")/*do not use i18n()!*/
                || text == QObject::tr("&Copy")
                || text == QObject::tr("&Paste")
                || text == QObject::tr("Delete")) {
            action->setEnabled(!readOnly);
        } else if (text == QObject::tr("&Redo")) {
//! @todo maybe redo will be enabled one day?
            action->setVisible(false);
        }
    }
}

//------------------

KexiSubwidgetInterface::KexiSubwidgetInterface()
{
}

KexiSubwidgetInterface::~KexiSubwidgetInterface()
{
}
