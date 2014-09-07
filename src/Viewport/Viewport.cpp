#include "Viewport/Viewport.h"

namespace Viewport {
    Viewport::Viewport() :
        _zoomFactor(2.0f) {

    }

    Viewport::Viewport(const ViewportRect & boundingRectNormalized,
                       const QSize & surfaceSize,
                       const ProjectionType & projectionType) :
        Viewport() {

        _surfaceSize = surfaceSize;
        _boundingRectNormalized = boundingRectNormalized;

        setProjectionType(projectionType);
    }

    Viewport::ProjectionType Viewport::projectionType() const {
        return _projectionType;
    }

    void Viewport::setProjectionType(const ProjectionType & projectionType) {
        _projectionType = projectionType;

        switch (_projectionType) {
            case PERSPECTIVE :
                perspective(60.0f, 1.0f, 0.0001f, 15.0f);
                lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));

                _orientationBillboard = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);

                _text = "perspective";
                break;
            case LEFT:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));

                _orientationBillboard = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f) *
                        QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f);

                _text = "left";
                break;
            case FRONTAL:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));

                _orientationBillboard = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f);

                _text = "frontal";
                break;
            case TOP:
                ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0001f, 10.0f);
                lookAt(QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));

                _orientationBillboard = QQuaternion();

                _text = "top";
                break;
        }

        emit projectionTypeChanged();
    }

    void Viewport::resize(const QSize & surfaceSize) {
        _surfaceSize = surfaceSize;
    }

    void Viewport::setBoundingRectNormalized(const QRectF & boundingRectNormalized) {
        QRectF clamped = QRectF(
                    std::min(std::max((float) boundingRectNormalized.x(), 0.0f), 1.0f),
                    std::min(std::max((float) boundingRectNormalized.y(), 0.0f), 1.0f),
                    boundingRectNormalized.width(),
                    boundingRectNormalized.height()
                    );

        clamped.setWidth(std::min(1.0f - clamped.x(), clamped.width()));
        clamped.setHeight(std::min(1.0f - clamped.y(), clamped.height()));

        _boundingRectNormalized = clamped;

        emit boundingRectNormalizedChanged();
    }

    QString Viewport::text() const {
        return _text;
    }

    ViewportRect Viewport::boundingRect() const {
        return ViewportRect(
                    _boundingRectNormalized.x() * _surfaceSize.width(),
                    _boundingRectNormalized.y() * _surfaceSize.height(),
                    _boundingRectNormalized.width() * _surfaceSize.width(),
                    _boundingRectNormalized.height() * _surfaceSize.height()
                    );
    }

    ViewportRect Viewport::boundingRectNormalized() const {
        return _boundingRectNormalized;
    }

    void Viewport::setBoundingRect(const QRect & boundingRect) {
        _boundingRectNormalized = boundingRect;
    }

    qreal Viewport::zoom() const {
        return _zoomFactor;
    }

    void Viewport::setZoom(const qreal & zoomFactor) {
        _pMatrix.setToIdentity();
        if (_projectionType == PERSPECTIVE) {
            // fov will be in 1/4 to 3/2 from initial fov
            float a = (16.0f - 5.0f * _eye.z()) / 5.0f;
            float b = (_eye.z() + a) / 4.0f;

            _pMatrix.perspective(_fov * (zoomFactor + b) / (_eye.z() + a), _aspectRatio, _nearVal, _farVal);
        }
        else if (zoomFactor != 0.0f) {
            _pMatrix.ortho(-zoomFactor / 2.0f, zoomFactor / 2.0f, -zoomFactor / 2.0f, zoomFactor / 2.0f, _nearVal, _farVal);
        }

        _zoomFactor = zoomFactor;

        emit zoomChanged();
    }

    QMatrix4x4 Viewport::projection() const {
        return _pMatrix;
    }

    QMatrix4x4 Viewport::view() const {
        return _vMatrix;
    }

    QMatrix4x4 Viewport::viewBillboard() const {
        return _vMatrixBillboard;
    }

    QMatrix4x4 Viewport::modelBillboard(const QMatrix4x4 & model) const {
        QMatrix4x4 modelMatrix(model);
        modelMatrix.rotate(_orientationBillboard);

        return modelMatrix;
    }

    bool Viewport::unproject(const QVector3D & projection, const QMatrix4x4 & mvp, QVector4D & unprojectedPoint) const {
        bool invertible;

        QMatrix4x4 inv = mvp.inverted(&invertible);

        if (!invertible) {
            return false;
        }

        QVector4D unprojectedPointVector = QVector4D(
                    2.0f * projection.x() - 1.0f,
                    2.0f * projection.y() - 1.0f,
                    2.0f * projection.z() - 1.0f,
                    1.0f);

        unprojectedPointVector = inv.map(unprojectedPointVector);

        if (unprojectedPointVector.w() == 0.0f) {
            return false;
        }
        else {
            unprojectedPoint = QVector4D(
                        unprojectedPointVector.x() / unprojectedPointVector.w(),
                        unprojectedPointVector.y() / unprojectedPointVector.w(),
                        unprojectedPointVector.z() / unprojectedPointVector.w(),
                        1.0f
            );

            return true;
        }
    }

    void Viewport::lookAt(const QVector3D & eye, const QVector3D & center, const QVector3D & up) {
        _vMatrix.setToIdentity();
        _vMatrixBillboard.setToIdentity();
        _vMatrix.lookAt(eye, center, up);

        if (_projectionType == PERSPECTIVE) {
            _vMatrixBillboard.lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
        }
        else {
            _vMatrixBillboard.lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
        }

        _eye = eye;
        _center = center;
        _up = up;
    }

    void Viewport::ortho(const float & left, const float & right, const float & bottom,
                            const float & top, const float & nearVal, const float & farVal) {
        _pMatrix.setToIdentity();
        _pMatrix.ortho(left, right, bottom, top, nearVal, farVal);

        _nearVal = nearVal;
        _farVal = farVal;
    }

    void Viewport::perspective(const float & fov, const float & aspectRatio, const float & nearVal, const float & farVal) {
        _pMatrix.setToIdentity();
        _pMatrix.perspective(fov, aspectRatio, nearVal, farVal);

        _fov = fov;
        _aspectRatio = aspectRatio;

        _nearVal = nearVal;
        _farVal = farVal;
    }
}
