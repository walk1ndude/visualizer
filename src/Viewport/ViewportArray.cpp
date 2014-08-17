#include "Viewport/ViewportArray.h"

static int viewPortId = 0;

namespace Viewport {
    ViewportArray::ViewportArray(const ViewportInfos & viewports, const QSize & windowSize) {
        setViewports(viewports, windowSize);
    }

    ViewportArray::ViewportArray() {
        QObject::connect(this, &ViewportArray::childrenChanged, [=]() {
            _viewportArray.clear();

            for (QQuickItem * child : childItems()) {
                if (Viewport * viewport = dynamic_cast<Viewport *>(child)) {
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

        QPair<QRectF, Viewport::ProjectionType> viewPortInfo;
        QVectorIterator<QPair<QRectF, Viewport::ProjectionType> >it(viewports);

        for (const QPair<QRectF, Viewport::ProjectionType> & viewportInfo : viewports) {
            _viewportArray.push_back(new Viewport(viewPortInfo.first, windowSize, viewPortInfo.second));
        }
    }

    void ViewportArray::resize(const QSize & surfaceSize) {
        for (Viewport * viewport : _viewportArray) {
            viewport->resize(surfaceSize);
        }
    }

    void ViewportArray::zoom(const qreal & zoomFactor) {
        for (Viewport * viewport : _viewportArray) {
            viewport->zoom(zoomFactor);
        }
    }

    bool ViewportArray::canRotate(const QPointF & startPos, const QPointF & finishPos) {
        // find viewport with perspective projection that contains both mouse positions
        /*foreach (Viewport * viewport, data()) {
            if (viewport->projectionType() == Viewport::PERSPECTIVE
                    && viewport->pointInViewport(startPos)
                    && viewport->pointInViewport(finishPos)) {
                return true;
            }
        }*/

        return false;
    }

    ViewPortInfoArray ViewportArray::viewportsInfo() {
        ViewPortInfoArray infoArray;

        for (Viewport * viewport : _viewportArray) {
            qDebug() << viewport->boundingRectNormalized();
            infoArray.append(ViewportInfo(viewport->boundingRectNormalized(), viewport->text()));
        }

        return infoArray;
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
        }

        return needToRedraw;
    }
}
