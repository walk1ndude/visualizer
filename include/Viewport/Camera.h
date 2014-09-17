#ifndef CAMERA_H
#define CAMERA_H

#include "Info/Info.h"

namespace Viewport {
    namespace Camera {
        using ProjectionMatrix = QMatrix4x4;
        using ViewMatrix = QMatrix4x4;
        using ModelMatrix = QMatrix4x4;

        using Eye = QVector3D;
        using Center = QVector3D;
        using Up = QVector3D;

        using Orientation = QQuaternion;

        using FOV = qreal;
        using AspectRatio = qreal;
        using NearPlane = qreal;
        using FarPlane = qreal;

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

            void lookAt(const Eye & eye, const Center & center, const Up & up);
            void lookAtBillboard(const Eye & eye, const Center & center, const Up & up,
                                 const Orientation & orientation = Orientation());

            void ortho(const Specs::Orthogonal & specs);
            void ortho(const AspectRatio & ratio);

            void perspective(const Specs::Perspective & specs);
            void perspective(const AspectRatio & ratio);

            ProjectionMatrix projection() const;

            ViewMatrix view() const;
            ViewMatrix viewBillboard() const;

            ModelMatrix model() const;
            ModelMatrix modelBillboard(const ModelMatrix & model) const;

            ZoomFactor zoomFactor() const;
            
            FOV zoomedFov(const FOV & fov) const;
            Top zoomedTop(const Top & top) const;

            Eye eye() const;
            Eye eyeBillboard() const;

            Orientation orientationBillboard() const;

            void reproject(const AspectRatio & ratio);

            static bool unproject(const QVector3D & projection, const QMatrix4x4 & mvp, QVector4D & unprojectedPoint);

            void zoom(const ZoomFactor & zoomFactor, const AspectRatio & ratio);

        private:
            QMutex _mutex;

            SpecsTagged _specs;

            ProjectionMatrix _pMatrix;

            ViewMatrix _vMatrix;

            ModelMatrix _mMatrix;

            Eye _eye;
            Center _center;
            Up _up;

            struct Billboard {
                Eye eye;
                Center center;
                Up up;

                ViewMatrix vMatrix;

                Orientation orientation;
            } _billboard;

            ZoomFactor _zoomFactor;
        };
    }
}

#endif // CAMERA_H
