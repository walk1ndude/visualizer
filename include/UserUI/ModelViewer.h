#ifndef SLICEVIEWER_H
#define SLICEVIEWER_H

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QVector3D>
#include <QtGui/QImage>

#include "Render/ModelRenderer.h"

namespace UserUI {
    class ModelViewer : public QQuickItem {        
        Q_PROPERTY(QSize fboSize READ fboSize WRITE setFboSize NOTIFY fboSizeChanged)

        Q_PROPERTY(QVariant message READ message WRITE recieve NOTIFY lastMessageChanged)

        Q_PROPERTY(Scene::ModelScene * modelScene READ modelScene WRITE setModelScene NOTIFY modelSceneChanged)
        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        QSize fboSize() const;

        Viewport::ViewportArray * viewportArray() const;

        Scene::ModelScene * modelScene() const;

        QVariant message() const;

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData * paintNodeData);

        void recieveMessage(const Message::SettingsMessage & message, const Message::SettingsMessage::PostCriteria & criteria);
        void postMessage(const Message::SettingsMessage & message, const Message::SettingsMessage::PostCriteria & criteria);

    private:
        Render::ModelRenderer * _modelRenderer;

        Viewport::ViewportArray * _viewportArray;

        QVector<Scene::ModelScene *> _modelScenes;

        QSize _fboSize;

        QQueue<Message::SettingsMessage> _messageQueue;

   signals:
        void viewportArrayChanged();

        void modelSceneChanged();

        void fboSizeChanged(const QSize & fboSize);

        void post(const Message::SettingsMessage & message);
        void post(const QVariantMap & message);

        void lastMessageChanged(const QVariant & message);

    public slots:
        virtual void recieve(const QVariant & message);

        virtual void setModelScene(Scene::ModelScene * modelScene);

        virtual void setFboSize(const QSize & fboSize);

        virtual void setViewportArray(Viewport::ViewportArray * viewportArray);
    };
}

#endif // SLICEVIEWER_H
