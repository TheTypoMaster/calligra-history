/*
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_RECORDED_PAINT_ACTION_EDITOR_H_
#define _KIS_RECORDED_PAINT_ACTION_EDITOR_H_

#include <QWidget>
#include "kis_recorded_action_editor_factory.h"

class QGridLayout;
class KisPaintOpSettingsWidget;
class KisRecordedPaintAction;

class KisRecordedPaintActionEditor : public QWidget
{
    Q_OBJECT
public:
    KisRecordedPaintActionEditor(QWidget* parent, KisRecordedAction* action);
    ~KisRecordedPaintActionEditor();
private slots:
    void configurationUpdated();
signals:
    void actionEdited();
private:
    KisRecordedPaintAction* m_action;
    QGridLayout* m_gridLayout;
    KisPaintOpSettingsWidget* m_configWidget;
};

class KisRecordedPaintActionEditorFactory : public KisRecordedActionEditorFactory
{
public:
    KisRecordedPaintActionEditorFactory();
    virtual ~KisRecordedPaintActionEditorFactory();
    virtual QWidget* createEditor(QWidget* parent, KisRecordedAction* action) const;
    virtual bool canEdit(const KisRecordedAction* action) const;
};

#endif