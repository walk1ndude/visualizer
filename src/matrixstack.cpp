#include "matrixstack.h"

void MatrixStack::identity(const QVector3D & position, const QVector3D & orientation) {
    _position = position;
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

void MatrixStack::zoomZ(const qreal & dist) {
    _pMatrix.setToIdentity();
    _pMatrix.ortho(-dist/2, dist/2, -dist/2, dist/2, 0.1, 10.0);
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

void MatrixStack::lookAt(const QVector3D & pos, const QVector3D & viewPoint, const QVector3D up) {
    _position = pos;
    _vMatrix.lookAt(pos, viewPoint, up);
}

void MatrixStack::ortho(const float & left, const float & right, const float & bottom,
                        const float & top, const float & near, const float & far) {
    _pMatrix.ortho(left, right, bottom, top, near, far);
}

void MatrixStack::perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal) {
    _pMatrix.perspective(fov, aspectRatio, nearVal, farVal);
}
