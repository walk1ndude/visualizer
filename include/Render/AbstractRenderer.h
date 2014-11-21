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

#include "Package/SettingsPackage.h"

namespace Render {
    class AbstractRenderer : public QThread {
        Q_OBJECT
    public:
        explicit AbstractRenderer(QOpenGLContext * context, const QSize & surfaceSize);
        virtual ~AbstractRenderer();

        virtual void setSurface(QOffscreenSurface * surface) final;
        virtual void selectScene(Scene::AbstractScene * scene = nullptr) final;

        virtual void setSurfaceSize(const QSize & surfaceSize) final;

    protected:
        QMutex renderMutex;

        virtual void render() = 0;

        virtual void connectWithScene(Scene::AbstractScene * scene);
        virtual void disconnectWithScene(Scene::AbstractScene * scene);

        virtual void cleanUp() final;

        virtual bool updateContent() final;
        virtual bool activateContext() final;

        Scene::AbstractScene * currentScene() const;

        QSize surfaceSize() const;

    private:
        bool _canRenderContent;
        bool _textureUpdateNeeded;
        bool _contentInitializeNeeded;

        QSize _surfaceSize;

        Scene::AbstractScene * _currectScene;

        QOpenGLFramebufferObject * _fboRender;
        QOpenGLFramebufferObject * _fboDisplay;

        QOffscreenSurface * _surface;

        QOpenGLContext * _context;

        // remember all scenes, rendered by this renderer -> for clean up after
        QSet<Scene::AbstractScene *> _sceneHistory;

    signals:
        void textureReady(const GLuint & fboTexId, const QSize & size);
        void contentToSaveRendered(const QImage & fboContent, const QRect & saveArea, const qreal & angle);
        void redraw();

    public slots:
        virtual void renderNext() final;
        virtual void shutDown() final;

        virtual void recievePackage(const Package::SettingsPackage & package) = 0;
    };
}
#endif // RENDERTHREAD_H
