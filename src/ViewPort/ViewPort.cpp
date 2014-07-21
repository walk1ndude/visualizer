#include "ViewPort/ViewPort.h"

namespace ViewPort {
    ViewPort::ViewPort() :
        _matrixStack(MatrixStack::PERSPECTIVE) {

    }

    ViewPort::ViewPort(const ViewPortRect & boundingRect,
                       const QSize & surfaceSize,
                       const ProjectionType & projectionType) :
        _matrixStack((projectionType == ViewPort::PERSPECTIVE) ? MatrixStack::PERSPECTIVE : MatrixStack::ORTHOGONAL),
        _surfaceSize(surfaceSize),
        _boundingRect(boundingRect),
        _projectionType(projectionType) {

        switch (projectionType) {
            case ViewPort::PERSPECTIVE :
                _matrixStack.perspective(60.0, 1.0, 0.0001, 10.0);
                _matrixStack.lookAt(QVector3D(0.0, 0.0, 2.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
                _matrixStack.rotate(QVector3D(-90.0, 0.0, 0.0));
                break;
            case ViewPort::LEFT:
                _matrixStack.ortho(-1.0, 1.0, -1.0, 1.0, 0.0001, 10.0);
                _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
                _matrixStack.rotate(QVector3D(-90.0, -90.0, 0.0));
                break;
            case ViewPort::FRONT:
                _matrixStack.ortho(-1.0, 1.0, -1.0, 1.0, 0.0001, 10.0);
                _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
                _matrixStack.rotate(QVector3D(-90.0, 0.0, 0.0));
                break;
            case ViewPort::TOP:
                _matrixStack.ortho(-1.0, 1.0, -1.0, 1.0, 0.0001, 10.0);
                _matrixStack.lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
                _matrixStack.rotate(QVector3D(0.0, -90.0, 0.0));
            break;
        }
    }

    ViewPort::~ViewPort() {

    }

    ViewPort::ProjectionType ViewPort::projectionType() const {
        return _projectionType;
    }

    void ViewPort::resize(const QSize & surfaceSize) {
        _surfaceSize = surfaceSize;
    }

    ViewPortRect ViewPort::boundingRect() const {
        return ViewPortRect(
                    _boundingRect.x() * _surfaceSize.width(),
                    _boundingRect.y() * _surfaceSize.height(),
                    _boundingRect.width() * _surfaceSize.width(),
                    _boundingRect.height() * _surfaceSize.height()
                    );
    }

    void ViewPort::setBoundingRect(const QRect & boundingRect) {
        _boundingRect = boundingRect;
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

    QMatrix4x4 ViewPort::model() const {
        return _matrixStack.model();
    }

    QMatrix4x4 ViewPort::projection() const {
        return _matrixStack.projection();
    }

    QMatrix4x4 ViewPort::view() const {
        return _matrixStack.view();
    }

    QMatrix3x3 ViewPort::normalM() const {
        return _matrixStack.normalM();
    }

    QVector4D ViewPort::mapToProjectionType(const QVector4D & vector) const {
        switch (_projectionType) {
        case TOP:
            return QVector4D(vector.x(), 0.0f, vector.y(), 1.0f);
        case LEFT:
            return QVector4D(0.0f, vector.y(), vector.x(), 1.0f);
        default:
            return QVector4D(vector.x(), vector.y(), 0.0f, 1.0f);
        }
    }

    bool ViewPort::calculateRayDir(const QPointF & point, QVector4D & rayDirection) const {
        float pX = point.x();
        float pY = point.y();

        float x = _boundingRect.x();
        float y = _boundingRect.y();

        float w = _boundingRect.width();
        float h = _surfaceSize.height();

        if (pX >= x && pY >= y && pX < x + w && pY < y + h) {
            qDebug() << mapToProjectionType(QVector4D(2.0 * (pX - x) / w - 1, - 2.0 * (pY - y) / h + 1, 0.0f, 1.0f));
            rayDirection = _matrixStack.calculateRayDir(
                            mapToProjectionType(QVector4D(2.0 * (pX - x) / w - 1, - 2.0 * (pY - y) / h + 1, 0.0f, 1.0f))
                        );
            return true;
        }
        else {
            return false;
        }
    }
}
