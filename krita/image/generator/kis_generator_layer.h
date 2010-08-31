/*
 *  Copyright (c) 2008 Boudewijn Rempt <boud@valdyas.org>
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
#ifndef KIS_GENERATOR_LAYER_H_
#define KIS_GENERATOR_LAYER_H_

#include "kis_selection_based_layer.h"
#include <krita_export.h>

class KisFilterConfiguration;

/**
 * A generator layer is a special kind of layer that can be prefilled
 * with some pixel pattern generated by a KisGenerator plugin.
 * A KisGenerator is similar to a filter, but doesn't take
 * input pixel data and creates new pixel data.
 *
 * It is not possible to destructively paint on a generator layer.
 *
 * XXX: what about threadedness?
 */
class KRITAIMAGE_EXPORT KisGeneratorLayer : public KisSelectionBasedLayer
{
    Q_OBJECT

public:
    /**
     * Create a new Generator layer with the given configuration
     * and selection. Note that the selection will be _copied_
     * (using COW, though).
     */
    KisGeneratorLayer(KisImageWSP image, const QString &name, KisFilterConfiguration * kfc, KisSelectionSP selection);
    KisGeneratorLayer(const KisGeneratorLayer& rhs);
    virtual ~KisGeneratorLayer();

    KisNodeSP clone() const {
        return KisNodeSP(new KisGeneratorLayer(*this));
    }

    KisFilterConfiguration *filter() const;
    void setFilter(KisFilterConfiguration * filterConfig);

    /**
     * Convenience functions.
     * FIXME: should be deprecated? (DK)
     */
    KisFilterConfiguration *generator() const {
        return filter();
    };
    void setGenerator(KisFilterConfiguration * filterConfig) {
        setFilter(filterConfig);
    };

    bool accept(KisNodeVisitor &);

    QIcon icon() const;
    KoDocumentSectionModel::PropertyList sectionModelProperties() const;

    /**
     * re-run the generator. This happens over the bounds
     * of the associated selection.
     */
    void update();
    
public slots:
    // KisIndirectPaintingSupport
    KisLayer* layer() {
        return this;
    }

private:
    class Private;
    Private * const m_d;
};

#endif

