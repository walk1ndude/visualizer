#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "Render/ViewPort.h"

namespace Render {
    using ViewPortInfo = QPair<QRectF, ViewPort::ProjectionType>;
    using ViewPorts = QVector<ViewPortInfo>;
    using ViewPortsIterator = QMutableVectorIterator<ViewPort>;

    class ViewPortArray : public QVector<ViewPort> {
    public:
        explicit ViewPortArray(const ViewPorts & viewPorts = ViewPorts(), const QSize & windowSize = QSize());

        void setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize);

        void scale(const QVector3D & scale);
        void rotate(qreal xRot, qreal yRot, qreal zRot);
        void zoom(const qreal & zoomFactor);

        void resize(const QSize & windowSize);
    };
}

#endif // VIEWPORTS_H
