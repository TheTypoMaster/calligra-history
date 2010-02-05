/*
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

#include "particle_brush.h"

#include "kis_paint_device.h"
#include "kis_random_accessor.h"

#include <KoColorSpace.h>
#include <KoColor.h>

const qreal TIME = 0.000030;

ParticleBrush::ParticleBrush() {
    m_properties = 0;
}

ParticleBrush::~ParticleBrush() {
}


void ParticleBrush::initParticles()
{
    m_particlePos.resize(m_properties->particleCount);
    m_particleNextPos.resize(m_properties->particleCount);
    m_accelaration.resize(m_properties->particleCount);
}

void ParticleBrush::setInitialPosition(QPointF pos) {
    for (int i = 0; i < m_properties->particleCount; i++)
    {
        m_particlePos[i] = pos;
        m_particleNextPos[i] = pos;
        m_accelaration[i] = (i + m_properties->iterations) * 0.5;
    }
}


void ParticleBrush::paintParticle(KisRandomAccessor& accWrite,KoColorSpace * cs, QPointF pos, const KoColor& color,qreal weight, bool respectOpacity)
{
    // opacity top left, right, bottom left, right
    KoColor myColor(color);
    quint8 opacity = respectOpacity? myColor.opacity() : OPACITY_OPAQUE;

    int ipx = int (pos.x());
    int ipy = int (pos.y());
    qreal fx = pos.x() - ipx;
    qreal fy = pos.y() - ipy;

    quint8 btl = qRound((1.0 - fx) * (1.0 - fy) * opacity * weight);
    quint8 btr = qRound((fx)  * (1.0 - fy) * opacity * weight);
    quint8 bbl = qRound((1.0 - fx) * (fy)  * opacity * weight);
    quint8 bbr = qRound((fx)  * (fy)  * opacity * weight);

    accWrite.moveTo(ipx  , ipy);
    myColor.setOpacity( qBound<quint16>(OPACITY_TRANSPARENT,btl + cs->alpha(accWrite.rawData()),OPACITY_OPAQUE) );
    memcpy(accWrite.rawData(), myColor.data(), cs->pixelSize());

    accWrite.moveTo(ipx + 1, ipy);
    myColor.setOpacity( qBound<quint16>(OPACITY_TRANSPARENT,btr + cs->alpha(accWrite.rawData()),OPACITY_OPAQUE) );
    memcpy(accWrite.rawData(), myColor.data(), cs->pixelSize());

    accWrite.moveTo(ipx, ipy + 1);
    myColor.setOpacity( qBound<quint16>(OPACITY_TRANSPARENT,bbl + cs->alpha(accWrite.rawData()),OPACITY_OPAQUE) );
    memcpy(accWrite.rawData(), myColor.data(), cs->pixelSize());

    accWrite.moveTo(ipx + 1, ipy + 1);
    myColor.setOpacity( qBound<quint16>(OPACITY_TRANSPARENT,bbr + cs->alpha(accWrite.rawData()),OPACITY_OPAQUE) );
    memcpy(accWrite.rawData(), myColor.data(), cs->pixelSize());
}




void ParticleBrush::draw(KisPaintDeviceSP dab,const KoColor& color,QPointF pos) {
    KisRandomAccessor accessor = dab->createRandomAccessor( qRound(pos.x()), qRound(pos.y()) );
    KoColorSpace * cs = dab->colorSpace();

    for (int i = 0; i < m_properties->iterations; i++) {
        for (int j = 0; j < m_properties->particleCount; j++) {
            /*
                m_time = 0.01;
                QPointF temp = m_position;
                QPointF dist = m_position - m_oldPosition;
                m_position = m_position + (dist + (m_acceleration*m_time*m_time));
                m_oldPosition = temp;
            */

            /*
                QPointF dist = info.pos() - m_position;
                dist *= 0.3; // scale
                dist *= 10; // force
                m_oldPosition += dist;
                m_oldPosition *= 0.989;
                m_position = m_position + m_oldPosition * m_time * m_time;
            */


            QPointF dist = pos - m_particlePos[j];
            dist.setX(dist.x() * m_properties->scale.x());
            dist.setY(dist.y() * m_properties->scale.y());
            dist = dist * m_accelaration[j];
            m_particleNextPos[j] = m_particleNextPos[j] + dist;
            m_particleNextPos[j] *= m_properties->gravity;
            m_particlePos[j] = m_particlePos[j] + (m_particleNextPos[j] * TIME);

            paintParticle(accessor, cs ,m_particlePos[j],color,m_properties->weight, true);

        }//for j
    }//for i
}


