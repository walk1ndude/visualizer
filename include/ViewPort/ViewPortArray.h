#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "ViewPort/Viewport.h"

#include "Model/AbstractModel.h"

#include <QtQuick/QQuickItem>

namespace Viewport {
    using ViewportInfos = QVector< QPair<QRectF, Viewport::ProjectionType> >;
    using ViewportsIterator = QVectorIterator<Viewport *>;

    class ViewportInfo {
    public:
        QRectF boundingRectNormalized;

        QString * text;

        ViewportInfo() { }

        ViewportInfo(const QRectF & boundingRectNormalized,
                     QString * text) :
            boundingRectNormalized(boundingRectNormalized),
            text(text) { }
    };

    using ViewPortInfoArray = QVector<ViewportInfo>;

    class ViewportArray : public QQuickItem {
        Q_OBJECT
    public:
        explicit ViewportArray();
        explicit ViewportArray(const ViewportInfos & viewports, const QSize & windowSize = QSize());
        ~ViewportArray();

        void setViewports(const ViewportInfos & viewports, const QSize & windowSize);

        void zoom(const qreal & zoomFactor);

        void resize(const QSize & surfaceSize);

        bool canRotate(const QPointF & startPos, const QPointF & finishPos);

        virtual ViewPortInfoArray viewportsInfo() final;

        void render(QListIterator<Model::AbstractModel *> & modelIterator);
        bool postProcess(QListIterator<Model::AbstractModel *> & modelIterator);

    private:
        QVector<Viewport *>  _viewportArray;

    signals:
        void viewportsChanged();
    };
}

#endif // VIEWPORTS_H
