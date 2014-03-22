#include "matrixstack.h"

void MatrixStack::initialize(const ViewPortParams & viewPortParams,
                             const QVector3D & cameraPos,
                             const QVector3D & rotationAngles,
                             const QVector3D & viewPoint,
                             const QVector3D & lookUpVector) {

    _viewPortParams = viewPortParams;
    _rotationAngles = rotationAngles;
    _cameraPos = cameraPos;
    _viewPoint = viewPoint;
    _lookUpVector = lookUpVector;

    _orientation = QVector3D(0.0, 0.0, 0.0);

    _cameraTransformation.setToIdentity();
    recalculateRotationMatrix();
}

QMatrix4x4 MatrixStack::mvpMatrix() {
    return  _pMatrix * _vMatrix * _mMatrix;
}

void MatrixStack::setAngles(const QVector3D & rotationAngles) {
    _rotationAngles = rotationAngles;

    QVector3D newOrientation = _orientation + _rotationAngles;

    float dX = newOrientation.x();
    float dY = newOrientation.y();
    float dZ = newOrientation.z();

    if (_orientation.x() >= -90.0 && dX <= -90.0) {
        _rotationAngles.setX(180.0 + (90.0 + dX));
    }
    else if (_orientation.x() <= 90.0 && dX >= 90.0) {
        _rotationAngles.setX(-180.0 + (dX - 90.0));
    }

    while (dX < -180.0 ) dX += 360.0;
    while (dX >= 180.0) dX -= 360.0;

    _orientation.setX(dX);

    qDebug() << _rotationAngles << _orientation << newOrientation;

    recalculateRotationMatrix();
}

void MatrixStack::setCameraPos(const QVector3D & cameraPos) {
    _cameraPos = cameraPos;
    recalculateCameraVectors();
}

void MatrixStack::setViewPoint(const QVector3D & viewPoint) {
    _viewPoint = viewPoint;
    recalculateCameraVectors();
}

void MatrixStack::setLookUpVector(const QVector3D & lookUpVector) {
    _lookUpVector = lookUpVector;
    recalculateCameraVectors();
}

void MatrixStack::identity() {
    _pMatrix.setToIdentity();
    _mMatrix.setToIdentity();
    _vMatrix.setToIdentity();
}

void MatrixStack::recalculateRotationMatrix() {
    _cameraTransformation.rotate(_rotationAngles.x(), 1.0, 0.0, 0.0);
    _cameraTransformation.rotate(_rotationAngles.y(), 0.0, 1.0, 0.0);
    _cameraTransformation.rotate(_rotationAngles.z(), 0.0, 0.0, 1.0);

    recalculateCameraVectors();
}

void MatrixStack::recalculateCameraVectors() {
    _cameraPosition = _cameraTransformation * _cameraPos;
    _cameraViewPoint = _cameraTransformation * _viewPoint;
    _cameraUpDirection = _cameraTransformation * _lookUpVector;

    recalculateStack();
}

void MatrixStack::recalculateStack() {
    identity();

    _pMatrix.ortho(_viewPortParams.left,
                   _viewPortParams.right,
                   _viewPortParams.bottom,
                   _viewPortParams.top,
                   _viewPortParams.nearVal,
                   _viewPortParams.farVal);

    _vMatrix.lookAt(_cameraPosition, _cameraViewPoint, _cameraUpDirection);
}
