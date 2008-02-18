/*
 *  Copyright (c) 2007 Emanuele Tamponi <emanuele@valinor.it>
 *
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

#ifndef KIS_RGB_TO_KS_COLOR_CONVERSION_TRANSFORMATION_H_
#define KIS_RGB_TO_KS_COLOR_CONVERSION_TRANSFORMATION_H_

#include <KoColorConversionTransformation.h>
#include <KoColorConversionTransformationFactory.h>

#include "kis_illuminant_profile.h"
#include "kis_ks_colorspace.h"

#include <gsl/gsl_vector.h>

template< typename _TYPE_, int _N_ >
class KisRGBToKSColorConversionTransformation : public KoColorConversionTransformation {

typedef KoColorConversionTransformation parent;
typedef KisKSColorSpaceTrait<_TYPE_,_N_> CSTrait;

public:
    KisRGBToKSColorConversionTransformation(const KoColorSpace *srcCs, const KoColorSpace *dstCs)
    : parent(srcCs, dstCs), m_rgbvec(0), m_ksvec(0), m_profile(0)
    {
        m_profile = static_cast<const KisIlluminantProfile*>(dstCs->profile());
        m_rgbvec = gsl_vector_calloc(  3  );
        m_ksvec  = gsl_vector_calloc(2*_N_);
    }

    ~KisRGBToKSColorConversionTransformation()
    {
        gsl_vector_free(m_rgbvec);
        gsl_vector_free(m_ksvec);
    }

    void transform(const quint8 *src8, quint8 *dst, int nPixels) const
    {
        // For each pixel we do this:
        // 1 - convert raw bytes to quint16
        // 2 - find reflectances using the transformation matrix of the profile.
        // 3 - convert reflectances to K/S

        const quint16 *src = reinterpret_cast<const quint16*>(src8);
        const int pixelSize = CSTrait::pixelSize;

        for ( ; nPixels > 0; nPixels-- ) {
            gsl_vector_set(m_rgbvec, 0, KoColorSpaceMaths<quint16,double>::scaleToA(src[2]));
            gsl_vector_set(m_rgbvec, 1, KoColorSpaceMaths<quint16,double>::scaleToA(src[1]));
            gsl_vector_set(m_rgbvec, 2, KoColorSpaceMaths<quint16,double>::scaleToA(src[0]));

            m_profile->fromRgb(m_rgbvec, m_ksvec);

            for (int i = 0; i < _N_; i++) {
                CSTrait::K(dst,i) = KoColorSpaceMaths<double,_TYPE_>::scaleToA(gsl_vector_get(m_ksvec,2*i+0));
                CSTrait::S(dst,i) = KoColorSpaceMaths<double,_TYPE_>::scaleToA(gsl_vector_get(m_ksvec,2*i+1));
            }
            CSTrait::nativealpha(dst) = KoColorSpaceMaths<quint16,_TYPE_>::scaleToA(src[3]);

            src += 4;
            dst += pixelSize;
        }
    }

protected:
    gsl_vector *m_rgbvec;
    gsl_vector *m_ksvec;

    const KisIlluminantProfile *m_profile;
};

template< typename _TYPE_, int _N_ >
class KisRGBToKSColorConversionTransformationFactory : public KoColorConversionTransformationFactory {

public:
    KisRGBToKSColorConversionTransformationFactory()
    : KoColorConversionTransformationFactory( RGBAColorModelID.id(),
                                              Integer16BitsColorDepthID.id(),
                                              QString("KS%1").arg(_N_),
                                              KisKSColorSpace<_TYPE_,_N_>::ColorDepthId().id() ) { return; }

    KoColorConversionTransformation *createColorTransformation( const KoColorSpace* srcColorSpace,
                                                                const KoColorSpace* dstColorSpace,
                                                                KoColorConversionTransformation::Intent renderingIntent = KoColorConversionTransformation::IntentPerceptual ) const
    {
        Q_UNUSED(renderingIntent);
        Q_ASSERT(canBeSource(srcColorSpace));
        Q_ASSERT(canBeDestination(dstColorSpace));
        return new KisRGBToKSColorConversionTransformation<_TYPE_,_N_>(srcColorSpace, dstColorSpace);
    }

    bool conserveColorInformation() const { return true; }
    bool conserveDynamicRange() const { return false; }

};

#endif // KIS_RGB_TO_KS_COLOR_CONVERSION_TRANSFORMATION_H_
