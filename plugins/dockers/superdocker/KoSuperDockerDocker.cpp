/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2009 Jeremy Lugagne <lugagne.jeremy@gmail.com>

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

#include "KoSuperDockerDocker.h"
#include <klocale.h>
#include <QTabWidget>
#include "KoDockContainer.h"
#include <QSizePolicy>
#include <QLayout>
#include <QDebug>
#include <KTabWidget>

KoSuperDockerDocker::KoSuperDockerDocker() : QDockWidget()
{
    setWindowTitle( i18n( "Super Docker" ) );

    m_tabWidget = new KTabWidget( );
    m_tabWidget->setCloseButtonEnabled(true);
    m_tabWidget->setTabCloseActivatePrevious(true);
    setWidget(m_tabWidget);
    m_tabWidget->setUpdatesEnabled(true);
    m_tabWidget->addTab(new KoDockContainer() , "Test1");
    m_tabWidget->addTab(new KoDockContainer() , "Test2");
    m_tabWidget->addTab(new KoDockContainer() , "Test3");
    m_tabWidget->addTab(new KoDockContainer() , "Test4");
    m_tabWidget->setTabPosition(QTabWidget::West);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(locationChanged( Qt::DockWidgetArea)));
    connect(m_tabWidget, SIGNAL(closeRequest(QWidget*)), this, SLOT(closeTab(QWidget*)));
}

KoSuperDockerDocker::~KoSuperDockerDocker()
{
    if(m_tabWidget){
        delete m_tabWidget;
    }
}

void KoSuperDockerDocker::closeTab(QWidget* widget)
{
    m_tabWidget->removeTab(m_tabWidget->indexOf(widget));
}

void KoSuperDockerDocker::locationChanged( Qt::DockWidgetArea location)
{   
    QTabWidget* tabWidget = dynamic_cast<QTabWidget*>(widget());
    if(tabWidget){
        if(location == Qt::BottomDockWidgetArea){
            tabWidget->setTabPosition(QTabWidget::North);
        }else if(location == Qt::LeftDockWidgetArea){
            tabWidget->setTabPosition(QTabWidget::East);
        }else if(location == Qt::RightDockWidgetArea){
            tabWidget->setTabPosition(QTabWidget::West);
        }else if(location == Qt::TopDockWidgetArea){
            tabWidget->setTabPosition(QTabWidget::South);
        }
    }
}

#include "KoSuperDockerDocker.moc"

