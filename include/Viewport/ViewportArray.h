#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "Viewport/Viewport.h"

namespace Viewport {
    using ViewportInfos = QVector< QPair<QRectF, Viewport::ProjectionType> >;
    using ViewportsIterator = QVectorIterator<Viewport *>;

    class ViewportArray : public QQuickItem {
        Q_OBJECT
    public:
        explicit ViewportArray();
        explicit ViewportArray(const ViewportInfos & viewports, const QSize & windowSize = QSize());
        ~ViewportArray();

        void setViewports(const ViewportInfos & viewports, const QSize & windowSize);

        Q_INVOKABLE void zoom(const qreal & zoomFactor, const qreal & x, const qreal & y, Viewport * viewport);

        Q_INVOKABLE void hideExcept(Viewport * viewport);
        Q_INVOKABLE void restore();

        void resize(const QSize & surfaceSize);

        QList<Viewport *> array() const;

    private:
        QList<Viewport *>  _viewportArray;

    signals:
        void viewportsUpdated();
    };
}

#endif // VIEWPORTS_H
