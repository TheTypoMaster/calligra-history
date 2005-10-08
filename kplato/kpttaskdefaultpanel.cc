/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kpttaskdefaultpanel.h"

#include <klocale.h>
#include <kcommand.h>
#include <klineedit.h>

#include <qlabel.h>

#include <kdebug.h>

namespace KPlato
{

KPTTaskDefaultPanel::KPTTaskDefaultPanel(KPTTask &task, KPTStandardWorktime *workTime, QWidget *parent, const char *n)
    : KPTTaskGeneralPanel(task, workTime, false, parent, n)
{
    namefield->hide();
    namelabel->hide();
    idfield->hide();
    idlabel->hide();
    wbsfield->hide();
    wbslabel->hide();
}


}  //KPlato namespace

#include "kpttaskdefaultpanel.moc"
