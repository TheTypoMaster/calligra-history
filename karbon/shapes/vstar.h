/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VSTAR_H__
#define __VSTAR_H__

#include "vcomposite.h"

/**
 * This shape offers star-like shapes with a lot of parameters :
 *
 * Types :
 *
 * Star           - fully connected star shape.
 * Star outline   - like star but without the cross connections.
 * Framed star    - like star outline but with an enclosing path.
 * Spoke          - basically a star outline with inner radius of zero.
 * Wheel          - like spoke but with enclosing path.
 * Polygon        - like VPolygon.
 * Gear           - variant on star outline, resembling the KDE gear.
 *
 * Parameters :
 *
 * Edges          - number of edges, which must be greater or equal to 3.
 * Outer radius   - radius amount of circumcircle of the star.
 * Inner radius   - inner radius where star has to connect to. This value
 *                  doesnt apply to polygon, spoke and wheel.
 * Inner angle    - extra radius amount for inner radius.
 * Roundness      - uses curves instead of lines for the star shape.
 */
class VStar : public VComposite
{
public:
	enum VStarType
	{
		star_outline,
		spoke,
		wheel,
		polygon,
		framed_star,
		star,
		gear
	};
	VStar( VObject* parent,
		const KoPoint& center, double outerRadius, double innerRadius,
		uint edges, double angle = 0.0, uint innerAngle = 0,
		double roundness = 0.0, VStarType type = star_outline );

	static double getOptimalInnerRadius( uint edges, double outerRadius, uint innerAngle );

	virtual QString name() const;

private:
	VStarType m_type;
};

#endif

