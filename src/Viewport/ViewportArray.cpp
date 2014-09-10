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

    void ViewportArray::zoom(const qreal & zoomFactor) {
        for (Viewport * viewport : _viewportArray) {
            viewport->setZoom(zoomFactor);
        }
    }

    void ViewportArray::render(QListIterator<Model::AbstractModel *> & modelIterator) {
        ViewportRect boundingRect;

        for (Viewport * viewport : _viewportArray) {
            boundingRect = viewport->boundingRect();
            glViewport(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());

            // draw each model
            while (modelIterator.hasNext()) {
                modelIterator.next()->drawModel(viewport);
            }

            modelIterator.toFront();
        }
    }

    bool ViewportArray::postProcess(QListIterator<Model::AbstractModel *> & modelIterator) {
        bool needToRedraw = false;

        while (modelIterator.hasNext()) {
            Model::AbstractModel * model = modelIterator.next();

            for (Viewport * viewport : _viewportArray) {
                needToRedraw |= model->checkDepthBuffer(viewport);
            }

            if (needToRedraw) {
                model->update();
            }
        }

        return needToRedraw;
    }
}
