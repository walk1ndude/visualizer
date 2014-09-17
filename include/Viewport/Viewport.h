#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtGui/QMatrix4x4>

#include <QtQuick/QQuickItem>

#include "Viewport/Camera.h"

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

        Camera::ModelMatrix modelBillboard(const Camera::ModelMatrix & model) const;

        Camera::ViewMatrix view() const;
        Camera::ViewMatrix viewBillboard() const;

        Camera::Eye eye() const;
        Camera::Eye eyeBillboard() const;

        QString text() const;

        qreal zoom() const;
        
        QQuaternion orientationBillboard() const;

    private:
        QSize _surfaceSize;

        QRectF _boundingRectNormalized;

        ProjectionType _projectionType;

        Camera::Camera * _camera;

        QQuaternion _orientationBillboard;

        QString _text;

    signals:
        void boundingRectNormalizedChanged();

        void zoomChanged();

    public slots:
        virtual void setBoundingRectNormalized(const QRectF & boundingRectNormalized) final;

        virtual void setBoundingRect(const QRect & boundingRect) final;

        virtual void setProjectionType(const ProjectionType & projectionType);

        virtual void setZoom(const qreal & zoomFactor) final;

        virtual void resize(const QSize & windowSize) final;
    };
}
#endif // VIEWPORT_H
