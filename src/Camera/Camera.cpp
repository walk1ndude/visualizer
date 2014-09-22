#include "Camera/Camera.h"

namespace Camera {
    Camera::Camera(const ZoomFactor & zoomFactor) :
        _zoomFactor(zoomFactor) {

    }

    void Camera::lookAt(const Eye & eye, const Center & center, const Up & up, const Delta & delta) {
        QMutexLocker locker(&_mutex);
        
        _delta += delta;

        _eye = eye + _delta;
        _center = center + _delta;
        _up = up;
        
        _vMatrix.setToIdentity();
        _vMatrix.lookAt(_eye, _center, _up);
    }

    void Camera::setOrientationBillboard(const Orientation & orientation, const Orientation & textureOrientation) {
        QMutexLocker locker(&_mutex);
        
        _billboard.orientation = orientation;

        _billboard.orientationMatrix.setToIdentity();
        _billboard.orientationMatrix.rotate(orientation);

        _billboard.orientationTexture.setToIdentity();
        _billboard.orientationTexture.rotate(textureOrientation);
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
    
    void Camera::setEye(const Eye & eye) {
        _eye = eye;
        _delta = Delta();
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

    ModelMatrix Camera::model() const {
        return _mMatrix;
    }

    ZoomFactor Camera::zoomFactor() const {
        return _zoomFactor;
    }

    Eye Camera::eye() const {
        return _eye;
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

    ModelMatrix Camera::modelBillboard() const {
        return _billboard.orientationMatrix;
    }

    ModelMatrix Camera::textureBillboard() const {
        return _billboard.orientationTexture;
    }
    
    Orientation Camera::orientationBillboard() const {
        return _billboard.orientation;
    }
}
