#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QtCore/QThread>

#include <QtGui/QVector3D>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

class RenderThread : public QThread {
    Q_OBJECT
public:
    RenderThread(QOpenGLContext * context, const QSize & surfaceSize);
    virtual ~RenderThread();

    void setSurface(QOffscreenSurface * surface);

protected:
    QSize _surfaceSize;

    QRectF _screenSaveRect;

    QVector3D _rotation;

    bool _takeShot;

    virtual void initialize() = 0;
    virtual void initializeViewPorts() = 0;

    virtual void render() = 0;

    virtual void updateTextures() = 0;

    virtual void cleanUp() = 0;

    bool updateContent();

private:
    bool _canRenderContent;
    bool _textureUpdateNeeded;
    bool _contentInitializeNeeded;

    QOpenGLFramebufferObject * _fboRender;
    QOpenGLFramebufferObject * _fboDisplay;

    QOffscreenSurface * _surface;

    QOpenGLContext * _context;

signals:
    void textureReady(const QImage & image, const QSize & size);
    void needToRedraw();
    void initialized();

public slots:
    void renderNext();
    void shutDown();
};

#endif // RENDERTHREAD_H
