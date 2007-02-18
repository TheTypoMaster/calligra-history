// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "Brush.h"

#include <KoGenStyles.h>
#include <KoOasisStyles.h>
#include <KoOasisContext.h>
#include <KoXmlNS.h>

KSpreadBrush::KSpreadBrush()
: m_gColor1( Qt::red )
, m_gColor2( Qt::green )
, m_gType( BCT_GHORZ )
, m_fillType( FT_BRUSH )
, m_unbalanced( false )
, m_xfactor( 100 )
, m_yfactor( 100 )
{
}


KSpreadBrush::KSpreadBrush( const QBrush &brush, const QColor &gColor1, const QColor &gColor2,
                    BCType gType, FillType fillType, bool unbalanced,
                    int xfactor, int yfactor )
: m_brush( brush )
, m_gColor1( gColor1 )
, m_gColor2( gColor2 )
, m_gType( gType )
, m_fillType( fillType )
, m_unbalanced( unbalanced )
, m_xfactor( xfactor )
, m_yfactor( yfactor )
{
}


KSpreadBrush & KSpreadBrush::operator=( const KSpreadBrush &brush )
{
    m_brush = brush.m_brush;
    m_gColor1 = brush.m_gColor1;
    m_gColor2 = brush.m_gColor2;
    m_gType = brush.m_gType;
    m_fillType = brush.m_fillType;
    m_unbalanced = brush.m_unbalanced;
    m_xfactor = brush.m_xfactor;
    m_yfactor = brush.m_yfactor;
    return *this;
}


void KSpreadBrush::saveOasisFillStyle( KoGenStyle &styleObjectAuto, KoGenStyles& mainStyles ) const
{
    switch ( m_fillType )
    {
        case FT_BRUSH:
        {
            if( m_brush.style() != Qt::NoBrush )
            {
                KoOasisStyles::saveOasisFillStyle( styleObjectAuto, mainStyles, m_brush );
            }
            else
            {
                styleObjectAuto.addProperty( "draw:fill","none" );
            }
            break;
        }
        case FT_GRADIENT:
            styleObjectAuto.addProperty( "draw:fill","gradient" );
#if 0
            styleObjectAuto.addProperty( "draw:fill-gradient-name", saveOasisGradientStyle( mainStyles ) );
#endif
            break;
    }
}


/*QString KSpreadBrush::saveOasisGradientStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle gradientStyle( KPrDocument::STYLE_GRADIENT no family name);
    gradientStyle.addAttribute( "draw:start-color", m_gColor1.name() );
    gradientStyle.addAttribute( "draw:end-color", m_gColor2.name() );

    QString unbalancedx( "50%" );
    QString unbalancedy( "50%" );

    if ( m_unbalanced )
    {
        unbalancedx = QString( "%1%" ).arg( m_xfactor / 4 + 50 );
        unbalancedy = QString( "%1%" ).arg( m_yfactor / 4 + 50 );
    }
    gradientStyle.addAttribute( "draw:cx", unbalancedx );
    gradientStyle.addAttribute( "draw:cy", unbalancedy );

    switch( m_gType )
    {
        case BCT_PLAIN:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GHORZ:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GVERT:
            gradientStyle.addAttribute( "draw:angle", 900 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GDIAGONAL1:
            gradientStyle.addAttribute( "draw:angle", 450 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GDIAGONAL2:
            gradientStyle.addAttribute( "draw:angle", 135 );
            gradientStyle.addAttribute( "draw:style", "linear" );
            break;
        case BCT_GCIRCLE:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "radial" );
            break;
        case BCT_GRECT:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "square" );
            break;
        case BCT_GPIPECROSS:
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", "axial" );
            break;
        case BCT_GPYRAMID: //todo fixme ! it doesn't work !
            gradientStyle.addAttribute( "draw:angle", 0 );
            gradientStyle.addAttribute( "draw:style", 0 );
            break;
    }

    return mainStyles.lookup( gradientStyle, "gradient" );
}*/


void KSpreadBrush::loadOasisFillStyle( KoOasisContext &context, const char * propertyType )
{
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( propertyType );

    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
    {
        const QString fill = styleStack.attributeNS( KoXmlNS::draw, "fill" );
        kDebug(33001) << " load object gradient fill type :" << fill << endl;

        if ( fill == "solid" || fill == "hatch" )
        {
            setBrush( KoOasisStyles::loadOasisFillStyle( styleStack, fill, context.oasisStyles() ) );
        }
        else if ( fill == "gradient" )
        {
            QString style = styleStack.attributeNS( KoXmlNS::draw, "fill-gradient-name" );
            QDomElement* draw = context.oasisStyles().drawStyles()[style];

            if ( draw )
            {
                setGColor1( draw->attributeNS( KoXmlNS::draw, "start-color", QString() ) );
                setGColor2( draw->attributeNS( KoXmlNS::draw, "end-color", QString() ) );

                QString type = draw->attributeNS( KoXmlNS::draw, "style", QString() );
                kDebug()<<" type :"<<type<<endl;
                if ( type == "linear" )
                {
                    int angle = draw->attributeNS( KoXmlNS::draw, "angle", QString() ).toInt() / 10;

                    // make sure the angle is between 0 and 359
                    angle = abs( angle );
                    angle -= ( (int) ( angle / 360 ) ) * 360;

                    // What we are trying to do here is to find out if the given
                    // angle belongs to a horizontal, vertical or diagonal gradient.
                    int lower, upper, nearAngle = 0;
                    for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
                    {
                        if ( upper >= angle )
                        {
                            int distanceToUpper = abs( angle - upper );
                            int distanceToLower = abs( angle - lower );
                            nearAngle = distanceToUpper > distanceToLower ? lower : upper;
                            break;
                        }
                    }
                    // nearAngle should now be one of: 0, 45, 90, 135, 180...
                    if ( nearAngle == 0 || nearAngle == 180 )
                        setGType( BCT_GHORZ ); // horizontal
                    else if ( nearAngle == 90 || nearAngle == 270 )
                        setGType( BCT_GVERT ); // vertical
                    else if ( nearAngle == 45 || nearAngle == 225 )
                        setGType( BCT_GDIAGONAL1 ); // diagonal 1
                    else if ( nearAngle == 135 || nearAngle == 315 )
                        setGType( BCT_GDIAGONAL2 ); // diagonal 2
                }
                else if ( type == "radial" || type == "ellipsoid" )
                    setGType( BCT_GCIRCLE ); // circle
                else if ( type == "square" || type == "rectangular" )
                    setGType( BCT_GRECT ); // rectangle
                else if ( type == "axial" )
                    setGType( BCT_GPIPECROSS ); // pipecross
                else //safe
                    setGType( BCT_PLAIN ); // plain

                // Hard to map between x- and y-center settings of ooimpress
                // and (un-)balanced settings of kpresenter. Let's try it.
                int x, y;
                if ( draw->hasAttributeNS( KoXmlNS::draw, "cx" ) )
                    x = draw->attributeNS( KoXmlNS::draw, "cx", QString() ).remove( '%' ).toInt();
                else
                    x = 50;

                if ( draw->hasAttributeNS( KoXmlNS::draw, "cy" ) )
                    y = draw->attributeNS( KoXmlNS::draw, "cy", QString() ).remove( '%' ).toInt();
                else
                    y = 50;

                if ( x == 50 && y == 50 )
                {
                    setGUnbalanced( false );
                    setGXFactor( 100 );
                    setGYFactor( 100 );
                }
                else
                {
                    setGUnbalanced( true );
                    // map 0 - 100% to -200 - 200
                    setGXFactor( 4 * x - 200 );
                    setGYFactor( 4 * y - 200 );
                }
            }

            // We have to set a brush with brushstyle != no background fill
            // otherwise the properties dialog for the object won't
            // display the preview for the gradient.
            QBrush tmpBrush;
            tmpBrush.setStyle( static_cast<Qt::BrushStyle>( 1 ) );
            setBrush( tmpBrush );
            setFillType( FT_GRADIENT );
        }
        else if ( fill == "none" )
        {
            //nothing
        }
        else if ( fill == "bitmap" )
        {
            //todo
            //not implementer in kpresenter...
            //the drawing object is filled with the bitmap specified by the draw:fill-image-name attribute.
            //QBrush implement setPixmap
            //easy just add pixmap and store it.
        }
    }
}
