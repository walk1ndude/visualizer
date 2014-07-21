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

    void ViewPortArray::rotate(qreal xRot, qreal yRot, qreal zRot) {
        for (int i = 0; i != size(); ++ i) {
            switch (data()[i].projectionType()) {
                case ViewPort::PERSPECTIVE:
                    data()[i].rotate(QVector3D(xRot - 90.0, yRot, zRot));
                    break;
                case ViewPort::TOP:
                    //data()[i].rotate(QVector3D(0.0, 0.0, zRot));
                    data()[i].rotate(QVector3D(0.0, -90 + yRot, 0.0));
                    break;
                case ViewPort::FRONT:
                    data()[i].rotate(QVector3D(xRot - 90.0, 0.0, 0.0));
                    break;
                case ViewPort::LEFT:
                    //data()[i].rotate(QVector3D(-90.0, yRot -90.0, 0.0));
                    data()[i].rotate(QVector3D(90.0, 0.0, yRot));
                    break;
            }
        }
    }

    QVector4D ViewPortArray::calculateRayDir(const QPointF & point) const {
        QVector4D rayDirection;

        for (int i = 0; !at(i).calculateRayDir(point, rayDirection) && i != size(); ++ i);

        return rayDirection;
    }
}
