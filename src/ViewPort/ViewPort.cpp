#include "ViewPort/ViewPort.h"

static int viewPortId = 0;

namespace ViewPort {
    ViewPort::ViewPort() {

    }

    ViewPort::ViewPort(const ViewPortRect & boundingRectNormalized,
                       const QSize & surfaceSize,
                       const ProjectionType & projectionType) :
        _id(viewPortId ++),
        _surfaceSize(surfaceSize),
        _boundingRectNormalized(boundingRectNormalized),
        _projectionType(projectionType) {

        switch (projectionType) {
            case ViewPort::PERSPECTIVE :
                perspective(60.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

                _qRotateVoxel = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);

                _text = "PERSPECTIVE";
                break;
            case ViewPort::LEFT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

                _qRotateVoxel = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f) *
                        QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 90.0f);

                _text = "LEFT";
                break;
            case ViewPort::FRONT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

                _qRotateVoxel = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);

                _text = "FRONT";
                break;
            case ViewPort::TOP:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));

                _text = "TOP";
                break;
        }
    }

    ViewPort::ProjectionType ViewPort::projectionType() const {
        return _projectionType;
    }

    void ViewPort::resize(const QSize & surfaceSize) {
        _surfaceSize = surfaceSize;
    }

    QString * ViewPort::text() {
        return &_text;
    }

    int ViewPort::id() {
        return _id;
    }

    ViewPortRect ViewPort::boundingRect() const {
        return ViewPortRect(
                    _boundingRectNormalized.x() * _surfaceSize.width(),
                    _boundingRectNormalized.y() * _surfaceSize.height(),
                    _boundingRectNormalized.width() * _surfaceSize.width(),
                    _boundingRectNormalized.height() * _surfaceSize.height()
                    );
    }

    ViewPortRect ViewPort::boundingRectNormalized() const {
        return _boundingRectNormalized;
    }

    void ViewPort::setBoundingRect(const QRect & boundingRect) {
        _boundingRectNormalized = boundingRect;
    }

    void ViewPort::zoom(const qreal & zoomFactor) {
        _pMatrix.setToIdentity();
        if (_projectionType == ViewPort::PERSPECTIVE) {
            // fov will be in 1/4 to 3/2 from initial fov
            float a = (16.0f - 5.0f * _eye.z()) / 5.0f;
            float b = (_eye.z() + a) / 4.0f;

            _pMatrix.perspective(_fov * (zoomFactor + b) / (_eye.z() + a), _aspectRatio, _nearVal, _farVal);
        }
        else if (zoomFactor != 0.0f) {
            _pMatrix.ortho(-zoomFactor / 2.0f, zoomFactor / 2.0f, -zoomFactor / 2.0f, zoomFactor / 2.0f, _nearVal, _farVal);
        }
    }

    QMatrix4x4 ViewPort::projection() const {
        return _pMatrix;
    }

    QMatrix4x4 ViewPort::view() const {
        return _vMatrix;
    }

    QMatrix4x4 ViewPort::viewVoxel() const {
        return _vMatrixVoxel;
    }

    QMatrix4x4 ViewPort::modelVoxel(const QMatrix4x4 & model) const {
        QMatrix4x4 modelMatrix(model);
        modelMatrix.rotate(_qRotateVoxel);

        return modelMatrix;
    }

    bool ViewPort::pointInViewPort(const QVector4D & point) const {
        return pointInViewPort(QPointF(point.x(), point.y()));
    }

    bool ViewPort::pointInViewPort(const QPointF & point) const {
        float pX = point.x();
        float pY = point.y();

        float x = _boundingRectNormalized.x() * _surfaceSize.width();
        float y = _boundingRectNormalized.y() * _surfaceSize.height();

        float w = _boundingRectNormalized.width() * _surfaceSize.width();
        float h = _boundingRectNormalized.height() * _surfaceSize.height();

        return (pX >= x && pY >= y && pX < x + w && pY < y + h);
    }

    bool ViewPort::unproject(const QVector4D & projection, QVector4D & unprojectedPoint) const {
        float x = _boundingRectNormalized.x() * _surfaceSize.width();
        float y = _boundingRectNormalized.y() * _surfaceSize.height();

        float w = _boundingRectNormalized.width() * _surfaceSize.width();
        float h = _boundingRectNormalized.height() * _surfaceSize.height();

        bool invertible;

        QMatrix4x4 invVP = (_pMatrix * _vMatrix).inverted(&invertible);

        if (!invertible) {
            return false;
        }

        QVector4D unprojectedPointVector = QVector4D(
                    2.0 * (projection.x() - x) / w - 1.0f,
                    - 2.0 * (projection.y() - y) / h - 1.0f,
                    projection.z(),
                    1.0f) * invVP;

        if (unprojectedPointVector.w() == 0.0f) {
            return false;
        }
        else {
            unprojectedPoint.setX(unprojectedPointVector.x() / unprojectedPointVector.w());
            unprojectedPoint.setY(unprojectedPointVector.y() / unprojectedPointVector.w());
            unprojectedPoint.setZ(unprojectedPointVector.z() / unprojectedPointVector.w());
            unprojectedPoint.setW(1.0f);
            return true;
        }
    }

    QVector3D ViewPort::placeXYZAccordingToViewPort(const QVector3D & xyz) {
        /* In different viewports axes have different meaning.
         * For example in "Left" z and x axes change their positions,
         * so x axis turns out to be the axis that determines the
         * distantion between model and camera, and z takes the role
         * of x in return
         */
        switch (_projectionType) {
        case ViewPort::LEFT:
            return QVector3D(xyz.z(), xyz.y(), xyz.x());
        case ViewPort::FRONT:
            return QVector3D(xyz.x(), xyz.z(), xyz.y());
        default:
            return xyz;
        }
    }

    void ViewPort::lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up) {
        _vMatrix.setToIdentity();
        _vMatrixVoxel.setToIdentity();
        _vMatrix.lookAt(eye, center, up);

        if (_projectionType == ViewPort::PERSPECTIVE) {
            _vMatrixVoxel.lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
        }
        else {
            _vMatrixVoxel.lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
        }

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
