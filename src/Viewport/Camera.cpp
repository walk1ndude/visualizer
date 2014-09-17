#include "Viewport/Camera.h"

namespace Viewport {
    namespace Camera {
        Camera::Camera(const ZoomFactor & zoomFactor) :
            _zoomFactor(zoomFactor) {

        }

        void Camera::lookAt(const Eye & eye, const Center & center, const Up & up) {
            QMutexLocker locker(&_mutex);

            _vMatrix.setToIdentity();
            _vMatrix.lookAt(eye, center, up);

            _eye = eye;
            _center = center;
            _up = up;
        }

        void Camera::lookAtBillboard(const Eye & eye, const Center & center, const Up & up, const Orientation & orientation) {
            QMutexLocker locker(&_mutex);

            _billboard.vMatrix.setToIdentity();
            _billboard.vMatrix.lookAt(eye, center, up);

            _billboard.eye = eye;
            _billboard.center = center;
            _billboard.up = up;
            _billboard.orientation = orientation;
        }

        void Camera::ortho(const Specs::Orthogonal & specs) {
            QMutexLocker locker(&_mutex);

            _pMatrix.setToIdentity();
            _pMatrix.ortho(specs.left, specs.right, specs.top, specs.bottom, specs.nearPlane, specs.farPlane);

            _specs.type = ORTHOGONAL;
            _specs.specs.orthogonal = specs;
        }

        void Camera::ortho(const AspectRatio & ratio) {
            QMutexLocker locker(&_mutex);

            if (_specs.type == ORTHOGONAL) {
                Specs::Orthogonal * specs = &_specs.specs.orthogonal;

                Top top = zoomedTop(specs->top);
                
                _pMatrix.setToIdentity();
                _pMatrix.ortho(top * ratio, - top * ratio, top, - top, specs->nearPlane, specs->farPlane);
            }
        }

        void Camera::perspective(const Specs::Perspective & specs) {
            QMutexLocker locker(&_mutex);

            _pMatrix.setToIdentity();
            _pMatrix.perspective(zoomedFov(specs.fov), specs.aspectRatio, specs.nearPlane, specs.farPlane);

            _specs.type = PERSPECTIVE;
            _specs.specs.perspective = specs;
        }

        void Camera::perspective(const AspectRatio & ratio) {
            QMutexLocker locker(&_mutex);

            if (_specs.type == PERSPECTIVE) {
                Specs::Perspective * specs = &_specs.specs.perspective;

                specs->aspectRatio = ratio;
                
                _pMatrix.setToIdentity();
                _pMatrix.perspective(zoomedFov(specs->fov), specs->aspectRatio, specs->nearPlane, specs->farPlane);
            }
        }
        
        FOV Camera::zoomedFov(const FOV & fov) const {
            float a = (16.0f - 5.0f * _eye.z()) / 5.0f;
            float b = (_eye.z() + a) / 4.0f;
            
            return fov * (_zoomFactor + b) / (_eye.z() + a);
        }
        
        Top Camera::zoomedTop(const Top & top) const {
            return _zoomFactor / (2 * top);
        }

        void Camera::zoom(const ZoomFactor & zoomFactor, const AspectRatio & ratio) {
            QMutexLocker locker(&_mutex);
            
            _pMatrix.setToIdentity();
            if (_specs.type == PERSPECTIVE) {
                // fov will be in 1/4 to 3/2 from initial fov
                Specs::Perspective * specs = &_specs.specs.perspective;
                
                _zoomFactor = zoomFactor;

                _pMatrix.perspective(zoomedFov(specs->fov), ratio, specs->nearPlane, specs->farPlane);
            }
            else if (zoomFactor != 0.0f) {
                Specs::Orthogonal * specs = &_specs.specs.orthogonal;
                
                _zoomFactor = zoomFactor;

                Top top = zoomedTop(specs->top);
                
                _pMatrix.ortho(top * ratio, - top * ratio, top, - top, specs->nearPlane, specs->farPlane);
            }
        }
        
        void Camera::reproject(const AspectRatio & ratio) {
            if (_specs.type == PERSPECTIVE) {
                perspective(ratio);
            }
            else {
                ortho(ratio);
            }
        }

        ProjectionMatrix Camera::projection() const {
            return _pMatrix;
        }

        ViewMatrix Camera::view() const {
            return _vMatrix;
        }

        ViewMatrix Camera::viewBillboard() const {
            return _billboard.vMatrix;
        }

        ModelMatrix Camera::model() const {
            return _mMatrix;
        }

        ZoomFactor Camera::zoomFactor() const {
            return _zoomFactor;
        }

        Eye Camera::eye() const {
            return _eye;
        }

        Eye Camera::eyeBillboard() const {
            return _billboard.eye;
        }

        Orientation Camera::orientationBillboard() const {
            return _billboard.orientation;
        }

        bool Camera::unproject(const QVector3D & projection, const QMatrix4x4 & mvp, QVector4D & unprojectedPoint) {
            bool invertible;

            QMatrix4x4 inv = mvp.inverted(&invertible);

            if (!invertible) {
                return false;
            }

            QVector4D unprojectedPointVector = QVector4D(
                        2.0f * projection.x() - 1.0f,
                        2.0f * projection.y() - 1.0f,
                        2.0f * projection.z() - 1.0f,
                        1.0f);

            unprojectedPointVector = inv.map(unprojectedPointVector);

            if (unprojectedPointVector.w() == 0.0f) {
                return false;
            }
            else {
                unprojectedPoint = QVector4D(
                            unprojectedPointVector.x() / unprojectedPointVector.w(),
                            unprojectedPointVector.y() / unprojectedPointVector.w(),
                            unprojectedPointVector.z() / unprojectedPointVector.w(),
                            1.0f
                );

                return true;
            }
        }
        
        ModelMatrix Camera::modelBillboard(const ModelMatrix & model) const {
            ModelMatrix modelMatrix(model);
            modelMatrix.rotate(_billboard.orientation);
            
            return modelMatrix;
        }
    }
}
