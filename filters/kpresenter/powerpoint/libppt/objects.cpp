/* libppt - library to read PowerPoint presentation
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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
 * Boston, MA 02110-1301, USA
*/

#include "objects.h"
#include "ustring.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <QColor>

using namespace Libppt;

class PropertyValue
{
public:
    enum { InvalidType,  IntType, DoubleType, StringType, BoolType, ColorType } type;
    bool b;
    int i;
    double d;
    std::string s;
    Color c;

    PropertyValue() {
        type = InvalidType;
        b = false;
        i = 0;
        d = 0.0;


    }
};

class Object::Private
{
public:
    int id;
    double top;
    double left;
    double width;
    double height;
    bool background;
    std::map<std::string, PropertyValue> properties;
};

Object::Object()
{
    d = new Private;
    d->id = -1;
    d->top = 0.0;
    d->left = 0.0;
    d->width = 10.0;
    d->height = 3.0;
    d->background = false;
}

Object::~Object()
{
    delete d;
}

int Object::id() const
{
    return d->id;
}

void Object::setId(int id)
{
    d->id = id;
}

double Object::top() const
{
    return d->top;
}

double Object::left() const
{
    return d->left;
}

double Object::width() const
{
    return d->width;
}

double Object::height() const
{
    return d->height;
}

void Object::setTop(double top)
{
    d->top = top;
}

void Object::setLeft(double left)
{
    d->left = left;
}

void Object::setWidth(double width)
{
    d->width = width;
}

void Object::setHeight(double height)
{
    d->height = height;
}

bool Object::isBackground() const
{
    return d->background;
}

void Object::setBackground(bool bg)
{
    d->background = bg;
}

bool Object::hasProperty(std::string name)
{
    std::map<std::string, PropertyValue>::const_iterator i;
    i = d->properties.find(name);
    if (i ==  d->properties.end())
        return false;
    else
        return true;
}

void Object::setProperty(std::string name, std::string value)
{
    PropertyValue pv;
    pv.type = PropertyValue::StringType;
    pv.s = value;
    d->properties[ name ] = pv;
}

void Object::setProperty(std::string name, int value)
{
    PropertyValue pv;
    pv.type = PropertyValue::IntType;
    pv.i = value;
    d->properties[ name ] = pv;
}

void Object::setProperty(std::string name, double value)
{
    PropertyValue pv;
    pv.type = PropertyValue::DoubleType;
    pv.d = value;
    d->properties[ name ] = pv;
}

void Object::setProperty(std::string name, bool value)
{
    PropertyValue pv;
    pv.type = PropertyValue::BoolType;
    pv.b = value;
    d->properties[ name ] = pv;
}

void Object::setProperty(std::string name, Color value)
{
    PropertyValue pv;
    pv.type = PropertyValue::ColorType;
    pv.c = value;
    d->properties[ name ] = pv;
}

int Object::getIntProperty(std::string name)
{
    PropertyValue pv;
    pv = d->properties[ name ];
    if (pv.type == PropertyValue::IntType)
        return pv.i;
    else
        return 0;
}

double Object::getDoubleProperty(std::string name)
{
    PropertyValue pv;
    pv = d->properties[ name ];
    if (pv.type == PropertyValue::DoubleType)
        return pv.d;
    else
        return 0;
}

bool Object::getBoolProperty(std::string name)
{
    PropertyValue pv;
    pv = d->properties[ name ];
    if (pv.type == PropertyValue::BoolType)
        return pv.b;
    else
        return false;

}

std::string Object::getStrProperty(std::string name)
{
    PropertyValue pv;
    pv = d->properties[ name ];
    if (pv.type == PropertyValue::StringType)
        return pv.s;
    else
        return "NoString";
}

Color Object::getColorProperty(std::string name)
{
    PropertyValue pv;
    pv = d->properties[ name ];
    if (pv.type == PropertyValue::ColorType)
        return pv.c;
    else
        return Color(153, 204, 255); // #99ccff

}

class TextObject::Private : public QSharedData
{
public:
    Private();
    ~Private();
    unsigned int type;

    /**
    * @brief Actual text for this object
    */
    QString text;

    /**
    * @brief Text style that applies to this object
    *
    */
    StyleTextPropAtom *atom;

    /**
    * @brief Extended text style that applies to this object
    *
    */
    StyleTextProp9Atom *atom9;

    /**
    * @brief Struct that contains precalculated style names based on
    * TextCFException and TextPFException combinations.
    *
    * For each individual character in this object's text three styles apply:
    * Paragraph style, List style and Character style. These are parsed from
    * TextCFException and TextPFException. For each character there is a
    * corresponding pair of TextCFException and TextPFException.
    *
    * Saving of styles is done before saving text contents so we'll cache
    * the style names and pairs of TextCFException and TextPFException.
    *
    *
    */
    struct StyleName {
        /**
        * @brief Text style name (e.g. T1)
        *
        */
        QString text;

        /**
        * @brief Paragraph style (e.g. P1)
        *
        */
        QString paragraph;

        /**
        * @brief List style (e.g. L1)
        *
        */
        QString list;

        /**
        * @brief TextCFException the style names are parsed from
        */
        const TextCFException *cf;

        /**
        * @brief TextPFException the style names are parsed from
        */
        const TextPFException *pf;
    };

    /**
    * @brief List of pre parsed style names that apply to this text
    */
    QList<StyleName> styleNames;
};


TextObject::Private::Private()
        : type(0)
        , atom(0)
        , atom9(0)
{

}

TextObject::Private::~Private()
{
    delete atom;
    delete atom9;
}


void TextObject::setStyleTextProperty(const StyleTextPropAtom *atom,
                                      const StyleTextProp9Atom *atom9)
{
    //We don't know the lifespan of this atom so we'll make a copy of it
    if (atom) {
        d->atom =  new StyleTextPropAtom(*atom);
    }

    if (atom9) {
        d->atom9 =  new StyleTextProp9Atom(*atom9);
    }
}


TextObject::TextObject(): Object()
{
    d = new Private;
}

TextObject::~TextObject()
{
}

unsigned int TextObject::type() const
{
    return d->type;
}

const char* TextObject::typeAsString() const
{
    switch (d->type) {
    case  Title       : return "Title";
    case  Body        : return "Body";
    case  Notes       : return "Notes";
    case  NotUsed     : return "NotUsed";
    case  Other       : return "Other";
    case  CenterBody  : return "CenterBody";
    case  CenterTitle : return "CenterTitle";
    case  HalfBody    : return "HalfBody";
    case  QuarterBody : return "QuarterBody";
    default: break;
    }

    return "Unknown";
}

void TextObject::addStylenames(const TextCFException *cf,
                               const TextPFException *pf,
                               const QString &text,
                               const QString &paragraph,
                               const QString &list)
{
    TextObject::Private::StyleName style;
    style.cf = cf;
    style.pf = pf;
    style.text = text;
    style.paragraph = paragraph;
    style.list = list;
    d->styleNames << style;
}

QString TextObject::textStyleName(const TextCFException *cf,
                                  const TextPFException *pf) const
{
    int pos = findStyle(cf, pf);
    if (pos >= 0) {
        return d->styleNames[pos].text;
    }

    return QString();
}

QString TextObject::paragraphStyleName(const TextCFException *cf,
                                       const TextPFException *pf) const
{
    int pos = findStyle(cf, pf);
    if (pos >= 0) {
        return d->styleNames[pos].paragraph;
    }

    return QString();
}

QString TextObject::listStyleName(const TextCFException *cf,
                                  const TextPFException *pf) const
{
    int pos = findStyle(cf, pf);
    if (pos >= 0) {
        return d->styleNames[pos].list;
    }

    return QString();
}

int TextObject::findStyle(const TextCFException *cf,
                          const TextPFException *pf) const
{
    for (int i = 0;i < d->styleNames.size();i++) {
        if (d->styleNames[i].cf == cf && d->styleNames[i].pf == pf) {
            return i;
        }
    }

    return -1;
}

StyleTextPropAtom *TextObject::styleTextProperty()
{
    return d->atom;
}

StyleTextProp9Atom *TextObject::styleTextProperty9()
{
    return d->atom9;
}

StyleTextProp9 *TextObject::findStyleTextProp9(TextCFException *cf)
{
    if (!d->atom || !d->atom9) {
        return 0;
    }

    for (unsigned int i = 0;i < d->atom->textCFRunCount();i++) {
        TextCFRun *run = d->atom->textCFRun(i);
        if (!run || !run->textCFException()) {
            continue;
        }

        if (run->textCFException() == cf) {
            return d->atom9->styleTextProp9(i);
        }
    }

    return 0;
}

void TextObject::setType(unsigned int type)
{
    d->type = type;
}

QString TextObject::text() const
{
    return d->text;
}


void TextObject::setText(const QString& text)
{
    d->text = text;
}


void TextObject::convertFrom(Object* object)
{
    setId(object->id());
    setLeft(object->left());
    setTop(object->top());
    setWidth(object->width());
    setHeight(object->height());

    if (object->isText()) {
        TextObject* textObj = static_cast<TextObject*>(object);
        setType(textObj->type());
        // jgn lupa diganti :  setText( textObj->text() );
    }
}

class GroupObject::Private
{
public:
    std::vector<Object*> objects;

    // in group nesting, coordinate transformations may occur
    // these values define the transformation
    double xoffset;
    double yoffset;
    double xscale;
    double yscale;

    double vx;
    double vy;
    double vwidth;
    double vheight;

    Private() : xoffset(0), yoffset(0), xscale(25.4 / 576), yscale(25.4 / 576),
            vx(0), vy(0), vwidth(0), vheight(0) {
    }
};

GroupObject::GroupObject()
{
    d = new Private;
}

GroupObject::~GroupObject()
{
    for (unsigned i = 0; i < d->objects.size(); i++)
        delete d->objects[i];
    delete d;
}

unsigned GroupObject::objectCount() const
{
    return d->objects.size();
}

Object* GroupObject::object(unsigned i)
{
    return d->objects[i];
}

void GroupObject::addObject(Object* object)
{
    d->objects.push_back(object);
}

void GroupObject::takeObject(Object* object)
{
    std::vector<Object*> result;
    for (unsigned i = 0; i < d->objects.size(); i++) {
        Object* obj = d->objects[i];
        if (obj != object)
            result.push_back(obj);
    }

    d->objects.clear();
    for (unsigned j = 0; j < result.size(); j++)
        d->objects.push_back(result[j]);
}

void
GroupObject::setViewportDimensions(double x, double y,
                                   double width, double height)
{
    d->vx = x;
    d->vy = y;
    d->vwidth = width;
    d->vheight = height;
}

void
GroupObject::setDimensions(double x, double y, double width, double height)
{
    if (width && height && d->vwidth && d->vheight) {
        // adapt the offset and scaling
        d->xoffset += d->xscale * x;
        d->xscale *= width / d->vwidth;
        d->xoffset -= d->xscale * d->vx;
        d->yoffset += d->yscale * y;
        d->yscale *= height / d->vheight;
        d->yoffset -= d->yscale * d->vy;
    }
}

double
GroupObject::getXOffset() const
{
    return d->xoffset;
}

double
GroupObject::getYOffset() const
{
    return d->yoffset;
}

double
GroupObject::getXScale() const
{
    return d->xscale;
}

double
GroupObject::getYScale() const
{
    return d->yscale;
}

class DrawObject::Private
{
public:
    unsigned shape;
    bool isVerFlip;
    bool isHorFlip;

    /**
    * @brief Name of the style corresponding this object
    *
    * This is usually generated by KoGenStyles
    */
    QString styleName;
};

DrawObject::DrawObject()
{
    d = new Private;
    d->shape = None;
}

DrawObject::~DrawObject()
{
    delete d;
}

unsigned DrawObject::shape() const
{
    return d->shape;
}

void DrawObject::setShape(unsigned s)
{
    d->shape = s;
}

bool DrawObject::isVerFlip() const
{
    return d->isVerFlip;
}

QString DrawObject::styleName() const
{
    return d->styleName;
}

void DrawObject::setVerFlip(bool isVerFlip)
{
    d->isVerFlip = isVerFlip;
}

bool DrawObject::isHorFlip() const
{
    return d->isHorFlip;
}

void DrawObject::setHorFlip(bool isHorFlip)
{
    d->isHorFlip = isHorFlip;
}

void DrawObject::setStyleName(const QString &name)
{
    d->styleName = name;
}

class TextFont::Private : public QSharedData
{
public:
    Private();
    ~Private();

    /**
    * @brief Font's name
    */
    QString fontName;

    /**
    * @brief Font's charset
    *
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    */
    int charset;

    /**
    * @brief Font's clip precision
    *
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    */
    int clipPrecision;

    /**
    * @brief Font's quality
    *
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    */
    int quality;

    /**
    * @brief Font's pitch and family
    *
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    */
    int pitchAndFamily;
};

TextFont::Private::Private()
        : charset(0)
        , clipPrecision(0)
        , quality(0)
        , pitchAndFamily(0)
{

}

TextFont::Private::~Private()
{
}


TextFont::TextFont()
{
    d = new Private();
}

TextFont::TextFont(const QString &fontName,
                   int charset,
                   int clipPrecision,
                   int quality,
                   int pitchAndFamily)
{
    d = new Private();
    d->fontName = fontName;
    d->charset = charset;
    d->clipPrecision = clipPrecision;
    d->quality = quality;
    d->pitchAndFamily = pitchAndFamily;
}

TextFont::TextFont(const TextFont &source)
        : d(source.d)
{
}


TextFont::~TextFont()
{
}

QString TextFont::name() const
{
    return d->fontName;
}

int TextFont::charset() const
{
    return d->charset;
}

int TextFont::clipPrecision() const
{
    return d->clipPrecision;
}

int TextFont::quality() const
{
    return d->quality;
}

int TextFont::pitchAndFamily() const
{
    return d->pitchAndFamily;
}



class TextFontCollection::Private
{
public:
    QList<TextFont> fonts;
};

TextFontCollection::TextFontCollection()
{
    d = new Private;
}

TextFontCollection::~TextFontCollection()
{
    delete d;
}

unsigned TextFontCollection::listSize() const
{
    return d->fonts.size();
}

void TextFontCollection::addFont(const TextFont &font)
{
    d->fonts << font;
}

TextFont* TextFontCollection::getFont(unsigned int index)
{
    if (index < listSize()) {
        return &d->fonts[index];
    }

    return 0;
}

