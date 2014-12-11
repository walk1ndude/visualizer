#ifndef CAMERA_H
#define CAMERA_H

#include "Info/Info.h"

#define INVERSE_QUAT(x) (x.conjugate() / x.lengthSquared())

namespace Camera {
    using ProjectionMatrix = QMatrix4x4;
    using ViewMatrix = QMatrix4x4;
    using ModelMatrix = QMatrix4x4;
    using ScaleMatrix = QMatrix4x4;
    using Matrix = QMatrix4x4;
    using NormalMatrix = QMatrix3x3;

    using Eye = QVector3D;
    using Center = QVector3D;
    using Up = QVector3D;
    using Delta = QVector3D;

    using Orientation = QQuaternion;
    using Rotation = QQuaternion;

    using FOV = qreal;
    using AspectRatio = qreal;
    using NearPlane = qreal;
    using FarPlane = qreal;

    using Delta = QVector3D;

    using Left = qreal;
    using Right = qreal;
    using Top = qreal;
    using Bottom = qreal;

    using ZoomFactor = qreal;

    enum Type {
        PERSPECTIVE = 0,
        ORTHOGONAL = 1
    };

    union Specs {
        struct Orthogonal {
            Left left;
            Right right;
            Top top;
            Bottom bottom;

            NearPlane nearPlane;
            FarPlane farPlane;
        } orthogonal;

        struct Perspective {
            FOV fov;
            AspectRatio aspectRatio;
            NearPlane nearPlane;
            FarPlane farPlane;
        } perspective;
    };

    class SpecsTagged {
    public:
        Type type;
        Specs specs;

        SpecsTagged() { }
    };

    class Camera {
    public:
        explicit Camera(const ZoomFactor & zoomFactor = 2.0f);

        void lookAt(const Eye & eye, const Center & center, const Up & up = Up(0.0f, 1.0f, 0.0f), const Delta & delta = Delta());
        void setOrientationBillboard(const Orientation & orientation = Orientation(),
                                     const Orientation & orientationTexture = Orientation());

        void ortho(const Specs::Orthogonal & specs);
        void perspective(const Specs::Perspective & specs);

        ProjectionMatrix projection() const;

        ViewMatrix view() const;

        ModelMatrix modelBillboard() const;
        ModelMatrix modelTextureBillboard() const;
        
        Orientation orientationBillboard() const;

        ZoomFactor zoomFactor() const;

        Eye eye() const;

        void reproject(const AspectRatio & ratio);

        static bool unproject(const PointsInfo::Position3D & projection, const Matrix & mvp, PointsInfo::Position3D & unprojectedPoint);

        void zoom(const ZoomFactor & zoomFactor, const AspectRatio & ratio);
        
        void setEye(const Eye & eye);

    private:
        QMutex _mutex;

        SpecsTagged _specs;

        ProjectionMatrix _pMatrix;

        ViewMatrix _vMatrix;

        Eye _eye;
        Center _center;
        Up _up;
        Delta _delta;

        struct Billboard {
            Orientation orientation;
            
            ModelMatrix modelBillboard;
            ModelMatrix modelTextureBillboard;
        } _billboard;

        ZoomFactor _zoomFactor;

        FOV zoomedFov(const FOV & fov) const;
        Top zoomedTop(const Top & top) const;

        void ortho(const AspectRatio & ratio);
        void perspective(const AspectRatio & ratio);
    };
}

#endif // CAMERA_H
