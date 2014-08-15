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
        qreal x;
        qreal y;
        QString * text;

        ViewPortLegend() { }

        ViewPortLegend(const int & id,
                       const QPointF & textPosition,
                       QString * text) :
            id(id),
            x(textPosition.x()),
            y(textPosition.y()),
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
