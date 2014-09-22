#include "Viewport/Viewport.h"

namespace Viewport {
    Viewport::Viewport() {
        _camera = new Camera::Camera(Camera::ZoomFactor(2.0f));
        
        QObject::connect(this, &QQuickItem::widthChanged, [=]() { _camera->reproject(width() / height()); });
        QObject::connect(this, &QQuickItem::heightChanged, [=]() { _camera->reproject(width() / height()); });
    }

    Viewport::Viewport(const ViewportRect & boundingRectNormalized,
                       const QSize & surfaceSize,
                       const ProjectionType & projectionType) :
        Viewport() {

        _surfaceSize = surfaceSize;
        _boundingRectNormalized = boundingRectNormalized;

        setProjectionType(projectionType);
    }

    Viewport::~Viewport() {
        delete _camera;
    }

    Viewport::ProjectionType Viewport::projectionType() const {
        return _projectionType;
    }
    
    void Viewport::initCamera(const Camera::Delta & delta) {
        switch (_projectionType) {
            case PERSPECTIVE:
                _camera->lookAt(Camera::Eye(0.0f, -2.0f, 0.0f), Camera::Center(0.0f, 0.0f, 0.0f), Camera::Up(0.0f, 0.0f, -1.0f), delta);
                _camera->setOrientationBillboard(Camera::Orientation::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f));
                break;
                
            case LEFT:
                _camera->lookAt(Camera::Eye(-1.0f, 0.0f, 0.0f), Camera::Center(0.0f, 0.0f, 0.0f), Camera::Up(0.0f, 0.0f, -1.0f), delta);
                _camera->setOrientationBillboard(Camera::Orientation::fromAxisAndAngle(0.0f, 1.0f, 0.0f, -90.0f),
                                                 Camera::Orientation::fromAxisAndAngle(1.0f, 0.0f, 0.0f, -180.0f));
                break;
                
            case FRONTAL:
                _camera->lookAt(Camera::Eye(0.0f, -1.0f, 0.0f), Camera::Center(0.0f, 0.0f, 0.0f), Camera::Up(0.0f, 0.0f, -1.0f), delta);
                _camera->setOrientationBillboard(Camera::Orientation::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 90.0f));
                break;

            case TOP:
                _camera->lookAt(Camera::Eye(0.0f, 0.0f, 1.0f), Camera::Center(0.0f, 0.0f, 0.0f), Camera::Up(0.0f, -1.0f, 0.0f), delta);
                break;
        }
    }
    
    void Viewport::initProjection() {
        Camera::Specs::Perspective perspectiveSpecs;
        
        perspectiveSpecs.fov = 75.0f;
        perspectiveSpecs.aspectRatio = 1.0f;
        perspectiveSpecs.nearPlane = 1.0f;
        perspectiveSpecs.farPlane = 15.0f;
        
        Camera::Specs::Orthogonal orthogonalSpecs;
        
        orthogonalSpecs.left = - 1.0f;
        orthogonalSpecs.right = 1.0f;
        orthogonalSpecs.top = - 1.0f;
        orthogonalSpecs.bottom = 1.0f;
        orthogonalSpecs.nearPlane = 0.01f;
        orthogonalSpecs.farPlane = 100.0f;
        
        switch (_projectionType) {
            case PERSPECTIVE:
                _camera->perspective(perspectiveSpecs);
                _text = "perspective";
                break;
                
            case LEFT:
                _camera->ortho(orthogonalSpecs);
                _text = "left";
                break;
                
            case FRONTAL:
                _camera->ortho(orthogonalSpecs);
                _text = "frontal";
                break;
            case TOP:
                _camera->ortho(orthogonalSpecs);
                _text = "top";
                break;
        }
    }

    void Viewport::setProjectionType(const ProjectionType & projectionType) {
        _projectionType = projectionType;
        
        initCamera();
        initProjection();
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
        return _camera->zoomFactor();
    }

    Camera::Eye Viewport::eye() const {
        return _camera->eye();
    }
    
    void Viewport::setZoom(const qreal & zoomFactor) {
        _camera->zoom(Camera::ZoomFactor(zoomFactor), Camera::AspectRatio(width() / height()));
        emit zoomChanged();
    }
    
    void Viewport::setZoom(const qreal & zoomFactor, const qreal & x, const qreal & y, Viewport * viewport) {
        if (this == viewport) {
            QVector3D pos = QVector3D(1.0f - x, 1.0f - y, 0.0f);
            QVector4D posWorld;
            Camera::Camera::unproject(pos, projection() * view(), posWorld);
            
            _camera->zoom(Camera::ZoomFactor(zoomFactor), Camera::AspectRatio(width() / height()));
            
            QVector4D posWorldZoomed;
            Camera::Camera::unproject(pos, projection() * view(), posWorldZoomed);
            
            QVector4D posDiff = (posWorldZoomed - posWorld) * 1.0f;

            initCamera(QVector3D(posDiff.x(), posDiff.y(), posDiff.z()));
        }
        else {
            _camera->zoom(Camera::ZoomFactor(zoomFactor), Camera::AspectRatio(width() / height()));
        }
        
        emit zoomChanged();
    }

    Camera::ProjectionMatrix Viewport::projection() const {
        return _camera->projection();
    }

    Camera::ViewMatrix Viewport::view() const {
        return _camera->view();
    }

    Camera::ModelMatrix Viewport::modelBillboard() const {
        return _camera->modelBillboard();
    }

    Camera::ModelMatrix Viewport::textureBillboardOrientation() const {
        return _camera->textureBillboard();
    }
    
    Camera::Orientation Viewport::orientationBillboard() const {
        return _camera->orientationBillboard();
    }
}
