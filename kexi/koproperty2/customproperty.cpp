/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "customproperty.h"
#include "sizepolicyedit.h"
#include "Property.h"

#include <QSize>
#include <QRect>
#include <QSizePolicy>
#include <QPoint>

#include <klocale.h>
#include <kdebug.h>

using namespace KoProperty;

CustomProperty::CustomProperty(Property *parent)
        : m_property(parent)
{
}

CustomProperty::~CustomProperty()
{
}

void
CustomProperty::emitPropertyChanged()
{
    m_property->emitPropertyChanged();
}

///////////////  SizeCustomProperty /////////////////////

SizeCustomProperty::SizeCustomProperty(Property *property)
        : CustomProperty(property)
{
    if (property && (property->type() == Size)) {
        QSize s = property->value().toSize();
        new Property("width", s.width(), i18n("Width"), i18n("Width"), Size_Width, property);
        new Property("height", s.height(), i18n("Height"), i18n("Height"), Size_Height, property);
    }
}

SizeCustomProperty::~SizeCustomProperty()
{}

bool
SizeCustomProperty::handleValue() const
{
    if (!m_property)
        return false;

    switch (m_property->type()) {
    case Size_Width: case Size_Height:
        return true;
    default:
        return false;
    }
}

void
SizeCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
    if (!m_property)
        return;

    if (m_property->parent()) {
        QSize s = m_property->parent()->value().toSize();

        if (m_property->type() == Size_Height)
            s.setHeight(value.toInt());
        else if (m_property->type() == Size_Width)
            s.setWidth(value.toInt());

        m_property->parent()->setValue(s, true, false);
    } else {
        QSize s = value.toSize();
        m_property->child("width")->setValue(s.width(), rememberOldValue, false);
        m_property->child("height")->setValue(s.height(), rememberOldValue, false);
    }
}

QVariant
SizeCustomProperty::value() const
{
    if (!m_property || !m_property->parent())
        return QVariant();

    if (m_property->type() == Size_Height)
        return m_property->parent()->value().toSize().height();
    else if (m_property->type() == Size_Width)
        return m_property->parent()->value().toSize().width();

    return QVariant();
}

///////////////  PointCustomProperty /////////////////////

PointCustomProperty::PointCustomProperty(Property *property)
        : CustomProperty(property)
{
    if (property && (property->type() == Point)) {
        QPoint p = property->value().toPoint();
        new Property("x", p.x(), i18n("X"), i18n("X"), Point_X, property);
        new Property("y", p.y(), i18n("Y"), i18n("Y"), Point_Y, property);
    }
}

PointCustomProperty::~PointCustomProperty()
{}

bool
PointCustomProperty::handleValue() const
{
    if (!m_property)
        return false;

    switch (m_property->type()) {
    case Point_X: case Point_Y:
        return true;
    default:
        return false;
    }
}

void
PointCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
    if (!m_property)
        return;

    if (m_property->parent()) {
        QPoint p = m_property->parent()->value().toPoint();

        if (m_property->type() == Point_X)
            p.setX(value.toInt());
        else if (m_property->type() == Point_Y)
            p.setY(value.toInt());

        m_property->parent()->setValue(p, true, false);
    } else {
        QPoint p = value.toPoint();
        m_property->child("x")->setValue(p.x(), rememberOldValue, false);
        m_property->child("y")->setValue(p.y(), rememberOldValue, false);
    }
}

QVariant
PointCustomProperty::value() const
{
    if (!m_property || !m_property->parent())
        return QVariant();

    if (m_property->type() == Point_X)
        return m_property->parent()->value().toPoint().x();
    else if (m_property->type() == Point_Y)
        return m_property->parent()->value().toPoint().y();

    return QVariant();
}

///////////////  RectCustomProperty /////////////////////

RectCustomProperty::RectCustomProperty(Property *property)
        : CustomProperty(property)
{
    if (property && (property->type() == Rect)) {
        QRect r = property->value().toRect();
        new Property("x", r.x(), i18n("X"), i18n("X"), Rect_X, property);
        new Property("y", r.y(), i18n("Y"), i18n("Y"), Rect_Y, property);
        new Property("width", r.width(), i18n("Width"), i18n("Width"), Rect_Width, property);
        new Property("height", r.height(), i18n("Height"), i18n("Height"), Rect_Height, property);
    }
}

RectCustomProperty::~RectCustomProperty()
{}

bool
RectCustomProperty::handleValue() const
{
    if (!m_property)
        return false;

    switch (m_property->type()) {
    case Rect_X: case Rect_Y: case Rect_Width: case Rect_Height:
        return true;
    default:
        return false;
    }
}

void
RectCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
    if (!m_property)
        return;

    if (m_property->parent()) {
        QRect r = m_property->parent()->value().toRect();

        if (m_property->type() == Rect_X) {
            //changing x component of Rect shouldn't change width
            const int delta = value.toInt() - r.x();
            r.setX(value.toInt());
            r.setWidth(r.width() + delta);
        } else if (m_property->type() == Rect_Y) {
            //changing y component of Rect shouldn't change height
            const int delta = value.toInt() - r.y();
            r.setY(value.toInt());
            r.setHeight(r.height() + delta);
        } else if (m_property->type() == Rect_Width)
            r.setWidth(value.toInt());
        else if (m_property->type() == Rect_Height)
            r.setHeight(value.toInt());

        m_property->parent()->setValue(r, true, false);
    } else {
        QRect r = value.toRect();
        m_property->child("x")->setValue(r.x(), rememberOldValue, false);
        m_property->child("y")->setValue(r.y(), rememberOldValue, false);
        m_property->child("width")->setValue(r.width(), rememberOldValue, false);
        m_property->child("height")->setValue(r.height(), rememberOldValue, false);
    }
}

QVariant
RectCustomProperty::value() const
{
    if (!m_property || !m_property->parent())
        return QVariant();

    if (m_property->type() == Rect_X)
        return m_property->parent()->value().toRect().x();
    else if (m_property->type() == Rect_Y)
        return m_property->parent()->value().toRect().y();
    else if (m_property->type() == Rect_Width)
        return m_property->parent()->value().toRect().width();
    else if (m_property->type() == Rect_Height)
        return m_property->parent()->value().toRect().height();

    return QVariant();
}


///////////////  SizePolicyCustomProperty /////////////////////

SizePolicyCustomProperty::SizePolicyCustomProperty(Property *property)
        : CustomProperty(property)
{
    if (property && (property->type() == SizePolicy)) {
        const QSizePolicy sp(property->value().value<QSizePolicy>());
        new Property("hSizeType", 
            new Property::ListData(
                SizePolicyDelegate::listData().keys, SizePolicyDelegate::listData().names),
                     (int)sp.horizontalPolicy(),
                     i18nc("Size Policy, in short", "Horz. Size Type"),
                     i18nc("Size Policy", "Horizontal Size Type"),
                     ValueFromList/*SizePolicy_HorizontalPolicy*/, property);
        new Property("vSizeType", 
            new Property::ListData(
                SizePolicyDelegate::listData().keys, SizePolicyDelegate::listData().names),
                     (int)sp.verticalPolicy(),
                     i18nc("Size Policy, in short", "Vert. Size Type"),
                     i18nc("Size Policy", "Vertical Size Type"),
                     ValueFromList/*SizePolicy_VerticalPolicy*/, property);
        new Property("hStretch",
                     sp.horizontalStretch(),
                     i18nc("Size Policy", "Horz. Stretch"),
                     i18nc("Size Policy", "Horizontal Stretch"),
                     Int, property);
//                     SizePolicy_HorizontalStretch, property);
        new Property("vStretch",
                     sp.verticalStretch(),
                     i18nc("Size Policy", "Vert. Stretch"),
                     i18nc("Size Policy", "Vertical Stretch"),
                     Int, property);
//                     SizePolicy_VerticalStretch, property);
    }
}

SizePolicyCustomProperty::~SizePolicyCustomProperty()
{
}

bool
SizePolicyCustomProperty::handleValue() const
{
    if (!m_property)
        return false;

    return m_property->type() != SizePolicy;
/*    switch (m_property->type()) {
    case SizePolicy_HorizontalPolicy:
    case SizePolicy_VerticalPolicy:
    case SizePolicy_HorizontalStretch:
    case SizePolicy_VerticalStretch:
        return true;
    default:
        return false;
    }*/
}

void
SizePolicyCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
    if (!m_property)
        return;

    if (m_property->parent()) {
        QSizePolicy v( m_property->parent()->value().value<QSizePolicy>() );

        if (m_property->name() == "hSizeType")
            v.setHorizontalPolicy(QSizePolicy::Policy(value.toInt()));
        else if (m_property->name() == "vSizeType")
            v.setVerticalPolicy(QSizePolicy::Policy(value.toInt()));
        else if (m_property->name() == "hStretch")
            v.setHorizontalStretch(value.toInt());
        else if (m_property->name() == "vStretch")
            v.setVerticalStretch(value.toInt());

        m_property->parent()->setValue(v, true, false);
    } else {
        const QSizePolicy v = value.value<QSizePolicy>();
        m_property->child("hSizeType")->setValue(v.horizontalPolicy(), rememberOldValue, false);
        m_property->child("vSizeType")->setValue(v.verticalPolicy(), rememberOldValue, false);
        m_property->child("hStretch")->setValue(v.horizontalStretch(), rememberOldValue, false);
        m_property->child("vStretch")->setValue(v.verticalStretch(), rememberOldValue, false);
    }
}

QVariant
SizePolicyCustomProperty::value() const
{
    if (!m_property || !m_property->parent())
        return QVariant();
    if (m_property->name() == "hSizeType")
        return m_property->parent()->value().value<QSizePolicy>().horizontalPolicy();
    else if (m_property->name() == "vSizeType")
        return m_property->parent()->value().value<QSizePolicy>().verticalPolicy();
    else if (m_property->name() == "hStretch")
        return m_property->parent()->value().value<QSizePolicy>().horizontalStretch();
    else if (m_property->name() == "vStretch")
        return m_property->parent()->value().value<QSizePolicy>().verticalStretch();
    return QVariant();
}
