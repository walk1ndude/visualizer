#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QMatrix4x4>

namespace ViewPort {
    using ViewPortRect = QRectF;

    enum ProjectionType {
        PERSPECTIVE = 0,
        LEFT = 1,
        FRONT = 2,
        TOP = 3
    };

    class ViewPort {
    public:
        explicit ViewPort();
        explicit ViewPort(const ViewPortRect & boundingRectNormalized,
                          const QSize & surfaceSize,
                          const ProjectionType & projectionType = LEFT);

        ViewPortRect boundingRect() const;
        ViewPortRect boundingRectNormalized() const;

        void setBoundingRect(const QRect & boundingRect);

        ProjectionType projectionType() const;

        void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up);

        void zoom(const qreal & zoomFactor);

        QMatrix4x4 modelVoxel(const QMatrix4x4 & model) const;

        QMatrix4x4 view() const;
        QMatrix4x4 viewVoxel() const;

        QMatrix4x4 projection() const;

        QString * text();

        int id();

        void resize(const QSize & windowSize);

        bool unproject(const QVector4D & projection, QVector4D & unprojectedPoint) const;

        bool pointInViewPort(const QVector4D & point) const;
        bool pointInViewPort(const QPointF & point) const;

        QVector3D placeXYZAccordingToViewPort(const QVector3D & xyz);

    private:
        int _id;

        QSize _surfaceSize;

        QRectF _boundingRectNormalized;

        ProjectionType _projectionType;

        QMatrix4x4 _vMatrix;
        QMatrix4x4 _vMatrixVoxel;

        QMatrix4x4 _pMatrix;

        QQuaternion _qRotateVoxel;

        QVector3D _orientation;
        QVector3D _eye;
        QVector3D _center;
        QVector3D _up;

        qreal _fov;

        qreal _aspectRatio;

        qreal _nearVal;
        qreal _farVal;

        QString _text;

        QVector4D calculateRayDir(const QVector4D & point) const;

        void ortho(const float & left, const float & right, const float & bottom,
                   const float & top, const float & nearVal, const float & farVal);

        void perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal);
    };
}
#endif // VIEWPORT_H
