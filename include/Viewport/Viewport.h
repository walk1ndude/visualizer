#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtCore/QQueue>

#include <QtGui/QMatrix4x4>

#include <QtQuick/QQuickItem>

#include "Camera/Camera.h"

namespace Viewport {
    using ViewportRect = QRectF;

    class Viewport : public QQuickItem {
        Q_PROPERTY(QRectF boundingRect READ boundingRectNormalized
                   WRITE normalizeAndSetBoundingRect NOTIFY boundingRectNormalizedChanged)

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

        qreal xRect() const;

        ProjectionType projectionType() const;

        Camera::ProjectionMatrix projection() const;

        Camera::Orientation orientationBillboard() const;
        
        Camera::ModelMatrix modelBillboard() const;
        Camera::ModelMatrix modelTextureBillboard() const;

        Camera::ViewMatrix view() const;

        Camera::Eye eye() const;
        
        QVector4D delta() const;

        QString text() const;

        qreal zoom() const;

        virtual void remember() final;
        virtual void restore() final;

    private:
        QSize _surfaceSize;

        QRectF _boundingRectNormalized;

        ProjectionType _projectionType;

        Camera::Camera * _camera;

        Camera::Orientation _orientationBillboard;
        
        QVector4D _delta;

        QString _text;

        QQueue<QRectF> _viewportRects;

    signals:
        void boundingRectNormalizedChanged();

        void zoomChanged();

    public slots:
        virtual void normalizeAndSetBoundingRect(const QRectF & boundingRectNormalized) final;

        virtual void setBoundingRect(const QRect & boundingRect) final;

        virtual void setProjectionType(const ProjectionType & projectionType);
        
        virtual void initCamera(const Camera::Delta & delta = Camera::Delta()) final;
        virtual void initProjection() final;

        virtual void setZoom(const qreal & zoomFactor) final;
        Q_INVOKABLE virtual void setZoom(const qreal & zoomFactor, const qreal & x, const qreal & y, Viewport * viewport);

        virtual void resize(const QSize & windowSize) final;
    };
}
#endif // VIEWPORT_H
