#include "ViewPort/ViewPort.h"

namespace ViewPort {
    ViewPort::ViewPort() {

    }

    ViewPort::ViewPort(const ViewPortRect & boundingRect,
                       const QSize & surfaceSize,
                       const ProjectionType & projectionType) :
        _surfaceSize(surfaceSize),
        _boundingRect(boundingRect),
        _projectionType(projectionType) {

        switch (projectionType) {
            case ViewPort::PERSPECTIVE :
                perspective(60.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, -2.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
                break;
            case ViewPort::LEFT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(-2.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
                break;
            case ViewPort::FRONT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, -3.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
                break;
            case ViewPort::TOP:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
            break;
        }
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
        _pMatrix.setToIdentity();
        if (_projectionType == ViewPort::PERSPECTIVE) {
            // fov will be in 1/4 to 3/2 from initial fov
            float a = (16.0f - 5.0f * _eye.z()) / 5.0f;
            float b = (_eye.z() + a) / 4.0f;

            _pMatrix.perspective(_fov * (zoomFactor + b) / (_eye.z() + a), _aspectRatio, _nearVal, _farVal);
        }
        else {
            _pMatrix.ortho(-zoomFactor / 2, zoomFactor / 2, -zoomFactor / 2, zoomFactor / 2, _nearVal, _farVal);
        }
    }

    QMatrix4x4 ViewPort::projection() const {
        return _pMatrix;
    }

    QMatrix4x4 ViewPort::view() const {
        return _vMatrix;
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
            rayDirection = calculateRayDir(
                            mapToProjectionType(QVector4D(2.0 * (pX - x) / w - 1, - 2.0 * (pY - y) / h + 1, 0.0f, 1.0f))
                        );
            return true;
        }
        else {
            return false;
        }
    }

    QVector4D ViewPort::calculateRayDir(const QVector4D & point) const {
        return point * (_pMatrix * _vMatrix).inverted() - _eye;
    }

    void ViewPort::lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up) {
        _vMatrix.setToIdentity();
        _vMatrix.lookAt(eye, center, up);

        _eye = eye;
        _center = center;
        _up = up;
    }

    void ViewPort::ortho(const float & left, const float & right, const float & bottom,
                            const float & top, const float & nearVal, const float & farVal) {
        _pMatrix.setToIdentity();
        _pMatrix.ortho(left, right, bottom, top, nearVal, farVal);

        _nearVal = nearVal;
        _farVal = farVal;
    }

    void ViewPort::perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal) {
        _pMatrix.setToIdentity();
        _pMatrix.perspective(fov, aspectRatio, nearVal, farVal);

        _fov = fov;
        _aspectRatio = aspectRatio;

        _nearVal = nearVal;
        _farVal = farVal;
    }
}
