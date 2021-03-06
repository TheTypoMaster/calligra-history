/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _METADATAEDITOR_H_
#define _METADATAEDITOR_H_

#include <QVariant>

#include <kparts/plugin.h>

class KisView2;

namespace KisMetaData
{
class Store;
}

/**
 * Template of view plugin
 */
class metadataeditorPlugin : public KParts::Plugin
{
    Q_OBJECT
public:
    metadataeditorPlugin(QObject *parent, const QVariantList &);
    virtual ~metadataeditorPlugin();

private slots:

    void slotEditLayerMetaData();

private:

    KisView2 * m_view;
};

#endif // metadataeditorPlugin_H
