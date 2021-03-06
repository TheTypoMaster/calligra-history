/*
 *  Copyright (c) 2004,2007-2009 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#include <cmath>

#include <QDomDocument>

#include "kis_fast_math.h"
#include "kis_circle_mask_generator.h"
#include "kis_base_mask_generator.h"

struct KisCircleMaskGenerator::Private {
    double xcoef, ycoef;
    double xfadecoef, yfadecoef;
};

KisCircleMaskGenerator::KisCircleMaskGenerator(qreal diameter, qreal ratio, qreal fh, qreal fv, int spikes)
        : KisMaskGenerator(diameter, ratio, fh, fv, spikes, CIRCLE, DefaultId), d(new Private)
{
    d->xcoef = 2.0 / width();
    d->ycoef = 2.0 / (KisMaskGenerator::d->ratio * width());
    d->xfadecoef = (KisMaskGenerator::d->fh == 0) ? 1 : (1.0 / (KisMaskGenerator::d->fh * width()));
    d->yfadecoef = (KisMaskGenerator::d->fv == 0) ? 1 : (1.0 / (KisMaskGenerator::d->fv * KisMaskGenerator::d->ratio * width()));
}

KisCircleMaskGenerator::~KisCircleMaskGenerator()
{
    delete d;
}

quint8 KisCircleMaskGenerator::valueAt(qreal x, qreal y) const
{
    if (KisMaskGenerator::d->empty) return 255;
    double xr = (x /*- m_xcenter*/);
    double yr = fabs(y /*- m_ycenter*/);

    if (KisMaskGenerator::d->spikes > 2) {
        double angle = (KisFastMath::atan2(yr, xr));

        while (angle > KisMaskGenerator::d->cachedSpikesAngle ){
            double sx = xr;
            double sy = yr;

            xr = KisMaskGenerator::d->cs * sx - KisMaskGenerator::d->ss * sy;
            yr = KisMaskGenerator::d->ss * sx + KisMaskGenerator::d->cs * sy;

            angle -= 2 * KisMaskGenerator::d->cachedSpikesAngle;
        }
    }

    double n = norme(xr * d->xcoef, yr * d->ycoef);

    if (n > 1) {
        return 255;
    } else {
        qreal transformedFadeX = d->xfadecoef * softness();
        qreal transformedFadeY = d->yfadecoef * softness();
        
        double normeFade = norme(xr * transformedFadeX, yr * transformedFadeY);
        if (normeFade > 1) {
            double xle, yle;
            // xle stands for x-coordinate limit exterior
            // yle stands for y-coordinate limit exterior
            // we are computing the coordinate on the external ellipse in order to compute
            // the fade value
            if (xr == 0) {
                xle = 0;
                yle = yr > 0 ? 1 / d->ycoef : -1 / d->ycoef;
            } else {
                double c = yr / (double)xr;
                xle = sqrt(1 / norme(d->xcoef, c * d->ycoef));
                xle = xr > 0 ? xle : -xle;
                yle = xle * c;
            }
            // On the internal limit of the fade area, normeFade is equal to 1
            double normeFadeLimitE = norme(xle * transformedFadeX, yle * transformedFadeY);
            return (uchar)(255 *(normeFade - 1) / (normeFadeLimitE - 1));
        } else {
            n = 1 - n;
            if( width() < 2 || height() < 2 || n > d->xcoef * 0.5 || n > d->ycoef * 0.5)
            {
              return 0;
            } else {
              return 255 *  ( 1 - 4 * n * n  / (d->xcoef * d->ycoef) );
            }
        }
    }
}

void KisCircleMaskGenerator::toXML(QDomDocument& d, QDomElement& e) const
{
    KisMaskGenerator::toXML(d, e);
    e.setAttribute("type", "circle");
}

void KisCircleMaskGenerator::setSoftness(qreal softness)
{
    if (softness == 0){
        KisMaskGenerator::setSoftness(1.0);
    }else{
        KisMaskGenerator::setSoftness(1.0 / softness);
    }
}
