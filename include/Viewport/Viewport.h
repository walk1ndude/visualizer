#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QMatrix4x4>

#include <QtQuick/QQuickItem>

#include "Camera/Camera.h"

namespace Viewport {
    using ViewportRect = QRectF;

    class Viewport : public QQuickItem {
        Q_PROPERTY(QRectF boundingRect READ boundingRectNormalized
                   WRITE setBoundingRectNormalized NOTIFY boundingRectNormalizedChanged)

        Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType)
        Q_PROPERTY(QString text READ text CONSTANT)

        Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

        Q_ENUMS(ProjectionType)

        Q_OBJECT
    public:
        enum ProjectionType {
            PERSPECTIVE = 0,
            LEFT = 1,
            FRONTAL = 2,
            TOP = 3
        };

        static bool unproject(const QVector3D & point, const QMatrix4x4 & mvp, QVector4D & unprojectedPoint);

        explicit Viewport();
        explicit Viewport(const ViewportRect & boundingRectNormalized,
                          const QSize & surfaceSize,
                          const ProjectionType & projectionType = PERSPECTIVE);

        ~Viewport();

        ViewportRect boundingRect() const;

        ViewportRect boundingRectNormalized() const;

        ProjectionType projectionType() const;

        Camera::ProjectionMatrix projection() const;

        Camera::ModelMatrix modelBillboard() const;
        Camera::ModelMatrix textureBillboardOrientation() const;

        Camera::ViewMatrix view() const;

        Camera::Eye eye() const;

        QString text() const;

        qreal zoom() const;

    private:
        QSize _surfaceSize;

        QRectF _boundingRectNormalized;

        ProjectionType _projectionType;

        Camera::Camera * _camera;

        Camera::Orientation _orientationBillboard;

        QString _text;

    signals:
        void boundingRectNormalizedChanged();

        void zoomChanged();

    public slots:
        virtual void setBoundingRectNormalized(const QRectF & boundingRectNormalized) final;

        virtual void setBoundingRect(const QRect & boundingRect) final;

        virtual void setProjectionType(const ProjectionType & projectionType);
        
        virtual void initCamera(const QVector3D & delta = QVector3D()) final;
        virtual void initProjection() final;

        virtual void setZoom(const qreal & zoomFactor) final;
        Q_INVOKABLE virtual void setZoom(const qreal & zoomFactor, const qreal & x, const qreal & y, Viewport * viewport);

        virtual void resize(const QSize & windowSize) final;
    };
}
#endif // VIEWPORT_H
