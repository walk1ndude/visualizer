#include <cmath>

#include "ViewPort/MatrixStack.h"

namespace ViewPort {
    MatrixStack::MatrixStack(ProjectionType projectionType) :
        _projectionType(projectionType) {
        identity();
    }

    void MatrixStack::identity(const QVector3D & eye, const QVector3D & orientation) {
        _eye = eye;
        _orientation = orientation;

        _pMatrix.setToIdentity();
        _vMatrix.setToIdentity();
        _mMatrix.setToIdentity();
    }

    QMatrix4x4 MatrixStack::model() const {
        return _mMatrix;
    }

    QMatrix4x4 MatrixStack::view() const {
        return _vMatrix;
    }

    QMatrix4x4 MatrixStack::projection() const {
        return _pMatrix;
    }

    QMatrix3x3 MatrixStack::normalM() const {
        return (_mMatrix * _vMatrix).normalMatrix();
    }

    void MatrixStack::zoom(const qreal & zoomFactor) {
        _pMatrix.setToIdentity();
        if (_projectionType == MatrixStack::PERSPECTIVE) {
            _pMatrix.perspective(_fov / (_eye.z() / zoomFactor), _aspectRatio, _nearVal, _farVal);
        }
        else {
            _pMatrix.ortho(-zoomFactor / 2, zoomFactor / 2, -zoomFactor / 2, zoomFactor / 2, _nearVal, _farVal);
        }
    }

    void MatrixStack::rotate(const QVector3D & angle) {
        QVector3D rot = _orientation - angle;

        _mMatrix.rotate(rot.x(), 1.0f, 0.0f, 0.0f);
        _mMatrix.rotate(rot.y(), 0.0f, 1.0f, 0.0f);
        _mMatrix.rotate(rot.z(), 0.0f, 0.0f, 1.0f);

        _orientation = angle;
    }

    QVector4D MatrixStack::calculateRayDir(const QVector4D & point) const {
        return point * (_pMatrix * _mMatrix * _vMatrix).inverted() - _eye;
    }

    void MatrixStack::lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D up) {
        _eye = eye;
        _center = center;
        _up = up;
        _vMatrix.setToIdentity();
        _vMatrix.lookAt(eye, center, up);
    }

    void MatrixStack::ortho(const float & left, const float & right, const float & bottom,
                            const float & top, const float & nearVal, const float & farVal) {
        _pMatrix.ortho(left, right, bottom, top, nearVal, farVal);

        _nearVal = nearVal;
        _farVal = farVal;
    }

    void MatrixStack::perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal) {
        _pMatrix.perspective(fov, aspectRatio, nearVal, farVal);

        _fov = fov;
        _aspectRatio = aspectRatio;

        _nearVal = nearVal;
        _farVal = farVal;
    }
}
