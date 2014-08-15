#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "ViewPort/ViewPort.h"

namespace ViewPort {
    using ViewPortInfo = QPair<QRectF, ViewPort::ProjectionType>;
    using ViewPorts = QVector<ViewPortInfo>;
    using ViewPortsIterator = QMutableVectorIterator<ViewPort>;

    class ViewPortLegend {
    public:
        int id;

        QRectF boundingRectNormalized;

        QString * text;

        ViewPortLegend() { }

        ViewPortLegend(const int & id,
                       const QRectF & boundingRectNormalized,
                       QString * text) :
            id(id),
            boundingRectNormalized(boundingRectNormalized),
            text(text) { }
    };

    using ViewPortLegendArray = QVector<ViewPortLegend>;

    class ViewPortArray : public QVector<ViewPort> {
    public:
        explicit ViewPortArray(const ViewPorts & viewPorts = ViewPorts(), const QSize & windowSize = QSize());

        void setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize);

        void zoom(const qreal & zoomFactor);

        void resize(const QSize & surfaceSize);

        bool canRotate(const QPointF & startPos, const QPointF & finishPos);

        virtual ViewPortLegendArray viewPortsLegend() final;
    };
}

#endif // VIEWPORTS_H
