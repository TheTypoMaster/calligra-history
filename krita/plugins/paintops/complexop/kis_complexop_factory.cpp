/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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

#include "kis_complexop_factory.h"

#include <KoInputDevice.h>
#include <kis_painter.h>
#include <kis_paintop_settings.h>
#include <kis_image.h>

#include "kis_complexop_settings_widget.h"
#include "kis_complexop_settings.h"
#include "kis_complexop.h"

KisComplexOpFactory::KisComplexOpFactory()
{
}


KisComplexOpFactory::~KisComplexOpFactory()
{
}


KisPaintOp * KisComplexOpFactory::createOp(const KisPaintOpSettingsSP settings,
        KisPainter * painter,
        KisImageWSP image)
{
    Q_UNUSED(image);

    const KisComplexOpSettings *complexopSettings = dynamic_cast<const KisComplexOpSettings *>(settings.data());
    Q_ASSERT(settings != 0 && complexopSettings != 0);

    KisPaintOp * op = new KisComplexOp(complexopSettings, painter);
    Q_CHECK_PTR(op);
    return op;
}

KisPaintOpSettingsSP KisComplexOpFactory::settings(const KoInputDevice& inputDevice, KisImageWSP image)
{
    Q_UNUSED(inputDevice);
    Q_UNUSED(image);
    return new KisComplexOpSettings();
}

KisPaintOpSettingsSP KisComplexOpFactory::settings(KisImageWSP image)
{
    Q_UNUSED(image);
    return new KisComplexOpSettings();
}

KisPaintOpSettingsWidget* KisComplexOpFactory::createSettingsWidget(QWidget* parent)
{
    return new KisComplexOpSettingsWidget(parent);
}