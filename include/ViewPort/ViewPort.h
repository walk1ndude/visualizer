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
            BOTTOM
        };

        explicit ViewPort();
        explicit ViewPort(const ViewPortRect & boundingRect,
                          const QSize & surfaceSize,
                          const ProjectionType & projectionType = ViewPort::LEFT);
        ~ViewPort();

        ViewPortRect boundingRect();
        void setBoundingRect(const QRect & boundingRect);

        ProjectionType projectionType();

        void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up);

        void zoom(const qreal & zoomFactor);
        void rotate(const QVector3D & angle);
        void scale(const QVector3D & scale);

        QMatrix4x4 model();
        QMatrix4x4 view();
        QMatrix4x4 projection();
        QMatrix4x4 scaleM();
        QMatrix3x3 normalM();

        void resize(const QSize & windowSize);

    private:
        MatrixStack _matrixStack;

        QSize _surfaceSize;

        QRectF _boundingRect;

        ProjectionType _projectionType;
    };
}
#endif // VIEWPORT_H
