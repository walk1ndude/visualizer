#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "ViewPort/MatrixStack.h"

namespace ViewPort {
    using ViewPortRect = QRectF;

    class ViewPort {
    public:

        enum ProjectionType {
            PERSPECTIVE,
            LEFT,
            FRONT,
            TOP
        };

        explicit ViewPort();
        explicit ViewPort(const ViewPortRect & boundingRect,
                          const QSize & surfaceSize,
                          const ProjectionType & projectionType = ViewPort::LEFT);
        ~ViewPort();

        ViewPortRect boundingRect() const;
        void setBoundingRect(const QRect & boundingRect);

        ProjectionType projectionType() const;

        void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up);

        void zoom(const qreal & zoomFactor);
        void rotate(const QVector3D & angle);
        void scale(const QVector3D & scale);

        QMatrix4x4 model() const;
        QMatrix4x4 view() const;
        QMatrix4x4 projection() const;
        QMatrix4x4 scaleM() const;
        QMatrix3x3 normalM() const;

        void resize(const QSize & windowSize);

        bool convertPointToWorldCoordintes(const QPointF & point, QVector4D & worldCoordinates) const;

    private:
        MatrixStack _matrixStack;

        QSize _surfaceSize;

        QRectF _boundingRect;

        ProjectionType _projectionType;
    };
}
#endif // VIEWPORT_H
