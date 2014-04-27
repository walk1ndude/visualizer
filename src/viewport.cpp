#include "viewport.h"

ViewPort::ViewPort() :
    _matrixStack(MatrixStack::PERSPECTIVE) {

}

ViewPort::ViewPort(const QRectF & boundingRect,
                   const QSize & windowSize,
                   const ProjectionType & projectionType) :
    _matrixStack((projectionType == ViewPort::PERSPECTIVE) ? MatrixStack::PERSPECTIVE : MatrixStack::ORTHOGONAL),
    _windowSize(windowSize),
    _boundingRect(boundingRect),
    _projectionType(projectionType) {

    switch (projectionType) {
        case ViewPort::PERSPECTIVE :
            _matrixStack.perspective(60.0, 1.0, 0.1, 10.0);
            _matrixStack.lookAt(QVector3D(0.0, 0.0, 2.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
            _matrixStack.rotate(QVector3D(-90.0, 0.0, 0.0));
            break;
        case ViewPort::LEFT:
            _matrixStack.ortho(-0.9, 0.9, -0.9, 0.9, 0.01, 10.0);
            _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
            _matrixStack.rotate(QVector3D(-90.0, -90.0, 0.0));
            break;
        case ViewPort::FRONT:
            _matrixStack.ortho(-0.9, 0.9, -0.9, 0.9, 0.01, 10.0);
            _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
            _matrixStack.rotate(QVector3D(-90.0, 0.0, 0.0));
            break;
        case ViewPort::BOTTOM:
            _matrixStack.ortho(-0.9, 0.9, -0.9, 0.9, 0.01, 10.0);
            _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
            break;
    }
}

ViewPort::~ViewPort() {

}

ViewPort::ProjectionType ViewPort::projectionType() {
    return _projectionType;
}

void ViewPort::resize(const QSize & windowSize) {
    _windowSize = windowSize;
}

QRectF ViewPort::boundingRect() {
    return QRectF(
                _boundingRect.x() * _windowSize.width(),
                _boundingRect.y() * _windowSize.height(),
                _boundingRect.width() * _windowSize.width(),
                _boundingRect.height() * _windowSize.height()
                );
}

void ViewPort::setBoundingRect(const QRect & boundingRect) {
    _boundingRect = boundingRect;
}

void ViewPort::scale(const QVector3D & scale) {
    _matrixStack.scale(scale);
}

void ViewPort::zoom(const qreal & zoomFactor) {
    _matrixStack.zoom(zoomFactor);
}

void ViewPort::rotate(const QVector3D & angle) {
    _matrixStack.rotate(angle);
}

void ViewPort::lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up) {
    _matrixStack.lookAt(eye, center, up);
}

QMatrix4x4 ViewPort::model() {
    return _matrixStack.model();
}

QMatrix4x4 ViewPort::projection() {
    return _matrixStack.projection();
}

QMatrix4x4 ViewPort::view() {
    return _matrixStack.view();
}

QMatrix4x4 ViewPort::scaleM() {
    return _matrixStack.scaleM();
}

QMatrix3x3 ViewPort::normalM() {
    return _matrixStack.normalM();
}
