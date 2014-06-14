#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <QtGui/QMatrix4x4>

namespace ViewPort {
    class MatrixStack {
    public:
        enum ProjectionType {
            PERSPECTIVE,
            ORTHOGONAL
        };

        explicit MatrixStack(ProjectionType projectionType);

        QMatrix4x4 model();
        QMatrix4x4 view();
        QMatrix4x4 projection();
        QMatrix4x4 scaleM();
        QMatrix3x3 normalM();

        void identity(const QVector3D & eye = QVector3D(0.0, 0.0, 0.0),
                      const QVector3D & orientation = QVector3D(0.0, 0.0, 0.0));

        void ortho(const float & left, const float & right, const float & bottom,
                   const float & top, const float & nearVal, const float & farVal);

        void perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal);

        void lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D up);

        void zoom(const qreal & zoomFactor);
        void rotate(const QVector3D & angle);
        void scale(const QVector3D & scale);

    private:
        QMatrix4x4 _mMatrix;
        QMatrix4x4 _vMatrix;
        QMatrix4x4 _pMatrix;
        QMatrix4x4 _sMatrix;

        QVector3D _orientation;
        QVector3D _eye;
        QVector3D _center;
        QVector3D _up;

        ProjectionType _projectionType;

        qreal _fov;

        qreal _aspectRatio;

        qreal _nearVal;
        qreal _farVal;
    };
}
#endif // MATRIXSTACK_H
