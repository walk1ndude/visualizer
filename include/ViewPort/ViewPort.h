#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QMatrix4x4>

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

        ViewPortRect boundingRect() const;
        void setBoundingRect(const QRect & boundingRect);

        ProjectionType projectionType() const;

        void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up);

        void zoom(const qreal & zoomFactor);

        QMatrix4x4 view() const;
        QMatrix4x4 projection() const;

        void resize(const QSize & windowSize);

        bool calculateRayDir(const QPointF & point, QVector4D & rayDirection) const;

    private:
        QSize _surfaceSize;

        QRectF _boundingRect;

        ProjectionType _projectionType;

        QMatrix4x4 _vMatrix;
        QMatrix4x4 _pMatrix;

        QVector3D _orientation;
        QVector3D _eye;
        QVector3D _center;
        QVector3D _up;

        qreal _fov;

        qreal _aspectRatio;

        qreal _nearVal;
        qreal _farVal;

        QVector4D mapToProjectionType(const QVector4D & vector) const;

        QVector4D calculateRayDir(const QVector4D & point) const;

        void ortho(const float & left, const float & right, const float & bottom,
                   const float & top, const float & nearVal, const float & farVal);

        void perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal);
    };
}
#endif // VIEWPORT_H
