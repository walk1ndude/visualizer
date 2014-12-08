#ifndef SLICEVIEWER_H
#define SLICEVIEWER_H

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QVector3D>
#include <QtGui/QImage>

#include "Render/ModelRenderer.h"

namespace Quick {
    class ModelViewer : public QQuickItem {        
        Q_PROPERTY(QVariantMap selectedPoint READ selectedPoint WRITE setSelectedPoint)

        Q_PROPERTY(QSize fboSize READ fboSize WRITE setFboSize NOTIFY fboSizeChanged)

        Q_PROPERTY(QVariant message READ message WRITE recieve NOTIFY lastMessageChanged)

        Q_PROPERTY(int modelID READ modelID WRITE setModelID NOTIFY modelIDChanged)

        Q_PROPERTY(Scene::ModelScene * modelScene READ modelScene WRITE setModelScene NOTIFY modelSceneChanged)

        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        QSize fboSize() const;
        
        QVariantMap selectedPoint() const;

        int modelID() const;

        Viewport::ViewportArray * viewportArray() const;

        Scene::ModelScene * modelScene() const;

        QVariant message() const;

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData * paintNodeData);

        virtual void recieveMessage(const Message::SettingsMessage & message);

    private:
        Render::ModelRenderer * _modelRenderer;

        Viewport::ViewportArray * _viewportArray;

        QVector<Scene::ModelScene *> _modelScenes;

        QVariantMap _selectedPoint;

        QSize _fboSize;

        uint _modelID;

   signals:
        void viewportArrayChanged();

        void pointAdded(const PointsInfo::Point & point);

        void pointUpdated(const QVariantMap & point);

        void modelIDChanged(const uint & modelID);

        void modelSceneChanged();

        void togglePointChanged(const QString & point);

        void fboSizeChanged(const QSize & fboSize);

        void post(const Message::SettingsMessage & message);
        void post(const QVariantMap & message);

        void lastMessageChanged(const QVariant & message);

    public slots:
        Q_INVOKABLE virtual void addPoint(const QPointF & point, Viewport::Viewport * viewport);
        Q_INVOKABLE virtual void togglePoint(const QString & point);

        virtual void recieve(const QVariant & message);

        virtual void setModelScene(Scene::ModelScene * modelScene);

        virtual void updatePoint(const PointsInfo::UpdatedPoint & point);

        virtual void setFboSize(const QSize & fboSize);

        virtual void setSelectedPoint(const QVariantMap & selectedPoint);

        virtual void setModelID(const int & modelID);
        virtual void setViewportArray(Viewport::ViewportArray * viewportArray);
    };
}

#endif // SLICEVIEWER_H
