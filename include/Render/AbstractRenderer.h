#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QtCore/QThread>

#include <QtGui/QVector3D>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

#include "Model/AbstractModel.h"

#include "Scene/ModelScene.h"

#include "Info/TextureInfo.h"

namespace Render {
    class FBOSaver : public QThread {
        Q_OBJECT
    public:
        void saveToDisk(const QImage & fboContent, const QRect &saveArea, const qreal & angle);
    };

    class AbstractRenderer : public QThread {
        Q_OBJECT
    public:
        explicit AbstractRenderer(QOpenGLContext * context, const QSize & surfaceSize);
        virtual ~AbstractRenderer();

        virtual void setSurface(QOffscreenSurface * surface) final;
        virtual void selectScene(Scene::AbstractScene * scene = nullptr) final;

    protected:
        QMutex _renderMutex;

        virtual void render() = 0;

        virtual void connectWithScene(Scene::AbstractScene * scene);
        virtual void disconnectWithScene(Scene::AbstractScene * scene);

        virtual void cleanUp() final;

        virtual bool updateContent() final;
        virtual bool activateContext() final;

        Scene::AbstractScene * selectedScene();

        QSize surfaceSize();

    private:
        bool _canRenderContent;
        bool _textureUpdateNeeded;
        bool _contentInitializeNeeded;

        QSize _surfaceSize;

        QRectF _screenSaveRect;

        bool _takeShot;

        Scene::AbstractScene * _selectedScene;

        QOpenGLFramebufferObject * _fboRender;
        QOpenGLFramebufferObject * _fboDisplay;

        QOffscreenSurface * _surface;

        QOpenGLContext * _context;

        // remember all scenes, rendered by this renderer -> for clean up after
        QSet<Scene::AbstractScene *> _sceneHistory;

        FBOSaver * _fboSaver;
    signals:
        void textureReady(const GLuint & fboTexId, const QSize & size);
        void contentToSaveRendered(const QImage & fboContent, const QRect & saveArea, const qreal & angle);
        void needToRedraw();
        void appearedSmthToDraw();

    public slots:
        virtual void renderNext() final;
        virtual void shutDown() final;
    };
}
#endif // RENDERTHREAD_H
