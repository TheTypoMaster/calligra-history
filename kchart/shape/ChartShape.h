/* This file is part of the KDE project

   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Inge Wallin     <inge@lysator.liu.se>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_CHART_SHAPE
#define KCHART_CHART_SHAPE

#include <KoShape.h>

#include "koChart.h"

#include "kchart_export.h"
#include "kchart_global.h"

#define ChartShapeId "ChartShape"


class QAbstractItemModel;


//enum ChartType { NoType, Bar, Line, Pie, Ring, Polar };

namespace KDChart
{
    class Chart;
    class AbstractDiagram;
}

namespace KChart
{

class KCHART_EXPORT ChartShape : public KoShape, public KoChart::ChartInterface
{
public:
    ChartShape();
    virtual ~ChartShape();

    KDChart::Chart* chart() const;

    /// reimplemented
    virtual void setModel( QAbstractItemModel* model );
    QAbstractItemModel *model();
    
    /// reimplemented
    virtual void paint( QPainter& painter, const KoViewConverter& converter );
    /// reimplemented
    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context );
    /// reimplemented
    virtual void saveOdf( KoShapeSavingContext & context ) const;

    OdfChartType    chartType() const;
    OdfChartSubtype chartSubtype() const;
    bool            threeDMode() const;

    OdfChartSubtype lastChartSubtype(OdfChartType type) const;

 private:
    void saveLegend( KoXmlWriter &bodyWriter,
		     KoGenStyles &mainStyles ) const;
    void saveOdfPlotArea( KoXmlWriter &xmlWriter,
                          KoGenStyles &mainStyles) const;
    void saveOdfData( KoXmlWriter& bodyWriter,
                      KoGenStyles& mainStyles ) const;


 public Q_SLOTS:
    /// Set new chart type and subtype.
    void setChartType( OdfChartType newType, 
		       OdfChartSubtype newSubtype = NormalChartSubtype );
    void setChartSubtype( OdfChartSubtype newSubtype );
    void toggleThreeDMode( bool );


 private:
    void setChartDefaults();
    void createDefaultData();

    //static bool  isCartesian( OdfChartType type );
    //static bool  isPolar( OdfChartType type );
    Q_DISABLE_COPY( ChartShape )

 private:
    class Private;
    Private * const d;

};

} // namespace KChart


#endif // KCHART_CHART_SHAPE
