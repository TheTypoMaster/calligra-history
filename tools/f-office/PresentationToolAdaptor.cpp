/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "PresentationToolAdaptor.h"

#include "MainWindow.h"
#include "PresentationTool.h"

#include <QTextDocument>
#include <QColor>
#include <QDBusAbstractAdaptor>

PresentationToolAdaptor::PresentationToolAdaptor( PresentationTool *tool )
: QDBusAbstractAdaptor(tool),
m_tool(tool)
{
    connect( m_tool, SIGNAL( startDrawPresentation(int ,int ) ), this, SIGNAL( startDrawPresentation(int ,int ) ) );
    connect( m_tool, SIGNAL( drawOnPresentation(int , int ) ), this, SIGNAL( drawOnPresentation (int , int ) ) );
    connect( m_tool, SIGNAL( highlightPresentation( int , int ) ), this, SIGNAL( highlightPresentation( int , int ) ) );
    connect( m_tool, SIGNAL( stopDrawPresentation() ), this, SIGNAL( stopDrawPresentation() ) );
    connect( m_tool, SIGNAL( normalPresentation() ), this, SIGNAL( normalPresentation() ) );
}

PresentationToolAdaptor::~PresentationToolAdaptor()
{
}
