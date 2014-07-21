#include "ViewPort/ViewPortArray.h"

namespace ViewPort {
    ViewPortArray::ViewPortArray(const ViewPorts & viewPorts, const QSize & windowSize) :
        QVector() {
        setViewPorts(viewPorts, windowSize);
    }

    void ViewPortArray::setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize) {
        clear();

        QVectorIterator<QPair<QRectF, ViewPort::ProjectionType> >it(viewPorts);
        QPair<QRectF, ViewPort::ProjectionType> viewPort;

        while (it.hasNext()) {
            viewPort = it.next();
            push_back(ViewPort(viewPort.first, windowSize, viewPort.second));
        }
    }

    void ViewPortArray::resize(const QSize & surfaceSize) {
        for (int i = 0; i != size(); ++ i) {
            data()[i].resize(surfaceSize);
        }
    }

    void ViewPortArray::zoom(const qreal & zoomFactor) {
        for (int i = 0; i != size(); ++ i) {
            data()[i].zoom(zoomFactor);
        }
    }

    QVector4D ViewPortArray::calculateRayDir(const QPointF & point) const {
        QVector4D rayDirection;

        for (int i = 0; !at(i).calculateRayDir(point, rayDirection) && i != size(); ++ i);

        return rayDirection;
    }
}
