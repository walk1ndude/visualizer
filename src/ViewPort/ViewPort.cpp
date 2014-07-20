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

    QMatrix4x4 ViewPort::model() const {
        return _matrixStack.model();
    }

    QMatrix4x4 ViewPort::projection() const {
        return _matrixStack.projection();
    }

    QMatrix4x4 ViewPort::view() const {
        return _matrixStack.view();
    }

    QMatrix4x4 ViewPort::scaleM() const {
        return _matrixStack.scaleM();
    }

    QMatrix3x3 ViewPort::normalM() const {
        return _matrixStack.normalM();
    }

    bool ViewPort::convertPointToWorldCoordintes(const QPointF & point, QVector4D & worldCoordinates) const {
        float pX = point.x();
        float pY = point.y();

        float x = _boundingRect.x();
        float y = _boundingRect.y();

        float w = _boundingRect.width();
        float h = _surfaceSize.height();

        if (pX >= x && pY >= y && pX < x + w && pY < y + h) {
            QVector4D wC = _matrixStack.convertPointToWorldCoordintes(
                            QVector4D(2.0 * (pX - x) / w - 1, - 2.0 * (pY - y) / h + 1, 0.0f, 1.0f)
                        );

            switch (_projectionType) {
            case TOP:
                worldCoordinates = QVector4D(wC.x(), 0.0f, wC.y(), 1.0f);
                break;
            case LEFT:
                worldCoordinates = QVector4D(0.0f, wC.y(), wC.x(), 1.0f);
                break;
            default:
                worldCoordinates = QVector4D(wC.x(), wC.y(), 0.0f, 1.0f);
                break;
            }
            return true;
        }
        else {
            return false;
        }
    }
}
