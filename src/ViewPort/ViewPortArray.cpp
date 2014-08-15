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

    bool ViewPortArray::canRotate(const QPointF & startPos, const QPointF & finishPos) {
        int i = 0;
        // find viewport with perspective projection that contains both mouse positions
        while (
               i < size()
               &&
               !(
               data()[i].projectionType() == ViewPort::PERSPECTIVE
               && data()[i].pointInViewPort(startPos)
               && data()[i].pointInViewPort(finishPos)
               )) {
            i ++;
        }

        return (i != size());
    }

    ViewPortLegendArray ViewPortArray::viewPortsLegend() {
        ViewPortLegendArray legendArray;

        for (int i = 0; i != size(); ++ i) {
            legendArray.append(ViewPortLegend(data()[i].id(), data()[i].boundingRectNormalized(), data()[i].text()));
        }

        return legendArray;
    }
}
