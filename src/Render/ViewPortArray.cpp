#include "Render/ViewPortArray.h"

namespace Render {
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

    void ViewPortArray::resize(const QSize & windowSize) {
        QMutableVectorIterator<ViewPort> it(*this);

        while (it.hasNext()) {
            it.next().resize(windowSize);
        }
    }

    void ViewPortArray::scale(const QVector3D & scale) {
        QMutableVectorIterator<ViewPort> it(*this);

        while (it.hasNext()) {
            it.next().scale(scale);
        }
    }

    void ViewPortArray::zoom(const qreal & zoomFactor) {
        QMutableVectorIterator<ViewPort> it(*this);

        while (it.hasNext()) {
            it.next().zoom(zoomFactor);
        }
    }

    void ViewPortArray::rotate(qreal xRot, qreal yRot, qreal zRot) {
        ViewPort viewPort;
        QMutableVectorIterator<ViewPort> it(*this);

        while (it.hasNext()) {
            viewPort = it.next();
            switch (viewPort.projectionType()) {
                case ViewPort::PERSPECTIVE:
                    viewPort.rotate(QVector3D(xRot - 90.0, yRot, zRot));
                    break;
                case ViewPort::BOTTOM:
                    viewPort.rotate(QVector3D(0.0, 0.0, zRot));
                    break;
                case ViewPort::FRONT:
                    viewPort.rotate(QVector3D(xRot - 90.0, 0.0, 0.0));
                    break;
                case ViewPort::LEFT:
                    viewPort.rotate(QVector3D(-90.0, yRot - 90.0, 0.0));
                    break;
            }
        }
    }
}
