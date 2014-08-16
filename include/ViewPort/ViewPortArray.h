#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "ViewPort/ViewPort.h"

namespace ViewPort {
    using ViewPorts = QVector< QPair<QRectF, ProjectionType> >;
    using ViewPortsIterator = QMapIterator<int, ViewPort *>;

    class ViewPortInfo {
    public:
        int id;

        QRectF boundingRectNormalized;

        QString * text;

        ViewPortInfo() { }

        ViewPortInfo(const int & id,
                       const QRectF & boundingRectNormalized,
                       QString * text) :
            id(id),
            boundingRectNormalized(boundingRectNormalized),
            text(text) { }
    };

    using ViewPortInfoArray = QVector<ViewPortInfo>;

    class ViewPortArray : public QMap<int, ViewPort *> {
    public:
        explicit ViewPortArray(const ViewPorts & viewPorts = ViewPorts(), const QSize & windowSize = QSize());
        ~ViewPortArray();

        void setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize);

        void zoom(const qreal & zoomFactor);

        void resize(const QSize & surfaceSize);

        bool canRotate(const QPointF & startPos, const QPointF & finishPos);

        virtual ViewPortInfoArray viewPortsInfo() final;
    };
}

#endif // VIEWPORTS_H
