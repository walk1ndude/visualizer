#include "matrixstack.h"

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
    _sMatrix.setToIdentity();
}

QMatrix4x4 MatrixStack::model() {
    return _mMatrix;
}

QMatrix4x4 MatrixStack::view() {
    return _vMatrix;
}

QMatrix4x4 MatrixStack::projection() {
    return _pMatrix;
}

QMatrix4x4 MatrixStack::scaleM() {
    return _sMatrix;
}

QMatrix3x3 MatrixStack::normalM() {
    return (_mMatrix * _vMatrix).normalMatrix();
}

void MatrixStack::zoom(const qreal & zoomFactor) {
    _pMatrix.setToIdentity();
    if (_projectionType == MatrixStack::PERSPECTIVE) {
        _pMatrix.perspective(_fov / fabs(_eye.z() / zoomFactor), _aspectRatio, _nearVal, _farVal);
    }
    else {
        _pMatrix.ortho(-zoomFactor / 2, zoomFactor / 2, -zoomFactor / 2, zoomFactor / 2, _nearVal, _farVal);
    }
}

void MatrixStack::scale(const QVector3D & scale) {
    _sMatrix.scale(scale);
}

float MatrixStack::anglePi(const float & angle) {
    float a = angle;

    while (a >= 180) {
        a -= 180;
    }
    while (a <= -180) {
        a += 180;
    }

    return a;
}

void MatrixStack::rotate(const QVector3D & angle) {
    QVector3D rot = _orientation - angle;

    _mMatrix.rotate(anglePi(rot.x()), 1.0, 0.0, 0.0);
    _mMatrix.rotate(anglePi(rot.y()), 0.0, 1.0, 0.0);
    _mMatrix.rotate(anglePi(rot.z()), 0.0, 0.0, 1.0);

    _orientation = angle;
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
