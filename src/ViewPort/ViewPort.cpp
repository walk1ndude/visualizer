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
                lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
                break;
            case ViewPort::LEFT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
                break;
            case ViewPort::FRONT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
                break;
            case ViewPort::TOP:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
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

    bool ViewPort::pointInViewPort(const QVector4D & point) const {
        float pX = point.x();
        float pY = point.y();

        float x = _boundingRect.x() * _surfaceSize.width();
        float y = _boundingRect.y() * _surfaceSize.height();

        float w = _boundingRect.width() * _surfaceSize.width();
        float h = _boundingRect.height() * _surfaceSize.height();

        return (pX >= x && pY >= y && pX < x + w && pY < y + h);
    }

    bool ViewPort::unproject(const QVector4D & projection, QVector4D & unprojectPoint) const {
        float x = _boundingRect.x() * _surfaceSize.width();
        float y = _boundingRect.y() * _surfaceSize.height();

        float w = _boundingRect.width() * _surfaceSize.width();
        float h = _boundingRect.height() * _surfaceSize.height();

        bool invertible;

        QMatrix4x4 invVP = (_pMatrix * _vMatrix).inverted(&invertible);

        if (!invertible) {
            return false;
        }

        QVector4D unprojectPointVector = QVector4D(
                    2.0 * (projection.x() - x) / w - 1.0f,
                    2.0 * (projection.y() - y) / h + 1.0f,
                    2.0 * projection.z() - 1.0f,
                    1.0f) * invVP;

        if (unprojectPointVector.w() == 0.0f) {
            return false;
        }
        else {
            unprojectPoint.setX(unprojectPointVector.x() / unprojectPointVector.w());
            unprojectPoint.setY(unprojectPointVector.y() / unprojectPointVector.w());
            unprojectPoint.setZ(unprojectPointVector.z() / unprojectPointVector.w());
            unprojectPoint.setW(1.0f);
            return true;
        }
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
