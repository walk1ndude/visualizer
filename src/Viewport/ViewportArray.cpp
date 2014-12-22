#include "Viewport/ViewportArray.h"

namespace Viewport {
    ViewportArray::ViewportArray(const ViewportInfos & viewports, const QSize & windowSize) {
        setViewports(viewports, windowSize);
    }

    ViewportArray::ViewportArray() {
        QObject::connect(this, &ViewportArray::childrenChanged, [=]() {
            _viewportArray.clear();

            for (QQuickItem * child : childItems()) {
                if (Viewport * viewport = qobject_cast<Viewport *>(child)) {
                    _viewportArray.append(viewport);
                }
            }
        });
    }

    ViewportArray::~ViewportArray() {
        _viewportArray.clear();
    }

    void ViewportArray::setViewports(const ViewportInfos & viewports, const QSize & windowSize) {
        _viewportArray.clear();

        for (const QPair<QRectF, Viewport::ProjectionType> & viewportInfo : viewports) {
            _viewportArray.push_back(new Viewport(viewportInfo.first, windowSize, viewportInfo.second));
        }
    }

    void ViewportArray::resize(const QSize & surfaceSize) {
        for (Viewport * viewport : _viewportArray) {
            viewport->resize(surfaceSize);
        }
    }

    void ViewportArray::zoom(const qreal & zoomFactor, const qreal & x, const qreal & y, Viewport * vp) {
        for (Viewport * viewport : _viewportArray) {
            if (vp == viewport) {
                viewport->setZoom(zoomFactor, x, y, vp);
            }
        }
        
        for (Viewport * viewport : _viewportArray) {
            if (vp != viewport) {
                viewport->setZoom(zoomFactor, x, y, vp);
            }
        }

        emit viewportsUpdated();
    }

    QList<Viewport *> ViewportArray::array() const {
        QList<Viewport *> array;

        for (Viewport * viewport : _viewportArray) {
            if (!viewport->boundingRect().isEmpty()) {
                array.append(viewport);
            }
        }

        return array;
    }

    void ViewportArray::hideExcept(Viewport * vp) {
        for (Viewport * viewport : _viewportArray) {
            viewport->remember();

            if (vp != viewport) {
                viewport->normalizeAndSetBoundingRect(QRectF(0.0f, 0.0f, 0.0f, 0.0f));
            }
            else {
                viewport->normalizeAndSetBoundingRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
            }
        }

        emit viewportsUpdated();
    }

    void ViewportArray::restore() {
        for (Viewport * viewport : _viewportArray) {
            viewport->restore();
        }

        emit viewportsUpdated();
    }
}
