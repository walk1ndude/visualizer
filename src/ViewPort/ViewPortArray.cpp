#include "ViewPort/ViewPortArray.h"

static int viewPortId = 0;

namespace ViewPort {
    ViewPortArray::ViewPortArray(const ViewPorts & viewPorts, const QSize & windowSize) :
        QMap<int, ViewPort *>() {
        setViewPorts(viewPorts, windowSize);
    }

    ViewPortArray::~ViewPortArray() {
        qDeleteAll(begin(), end());
    }

    void ViewPortArray::setViewPorts(const ViewPorts & viewPorts, const QSize & windowSize) {
        clear();

        QPair<QRectF, ProjectionType> viewPortInfo;
        QVectorIterator<QPair<QRectF, ProjectionType> >it(viewPorts);

        while (it.hasNext()) {
            viewPortInfo = it.next();
            insert(viewPortId ++, new ViewPort(viewPortInfo.first, windowSize, viewPortInfo.second));
        }
    }

    void ViewPortArray::resize(const QSize & surfaceSize) {
        foreach (ViewPort * viewPort, values()) {
            viewPort->resize(surfaceSize);
        }
    }

    void ViewPortArray::zoom(const qreal & zoomFactor) {
        foreach (ViewPort * viewPort, values()) {
            viewPort->zoom(zoomFactor);
        }
    }

    bool ViewPortArray::canRotate(const QPointF & startPos, const QPointF & finishPos) {
        // find viewport with perspective projection that contains both mouse positions
        foreach (ViewPort * viewPort, values()) {
            if (viewPort->projectionType() == PERSPECTIVE
                    && viewPort->pointInViewPort(startPos)
                    && viewPort->pointInViewPort(finishPos)) {
                return true;
            }
        }

        return false;
    }

    ViewPortInfoArray ViewPortArray::viewPortsLegend() {
        ViewPortInfoArray infoArray;

        foreach (const int & key, keys()) {
            infoArray.append(ViewPortInfo(key, value(key)->boundingRectNormalized(), value(key)->text()));
        }

        return infoArray;
    }
}
