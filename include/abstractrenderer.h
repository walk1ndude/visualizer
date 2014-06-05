#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QtCore/QThread>

#include <QtGui/QVector3D>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

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

    virtual bool updateContent() final;

private:
    bool _canRenderContent;
    bool _textureUpdateNeeded;
    bool _contentInitializeNeeded;

    QOpenGLFramebufferObject * _fboRender;
    QOpenGLFramebufferObject * _fboDisplay;

    QOffscreenSurface * _surface;

    QOpenGLContext * _context;
    
    QMutex _renderMutex;

    FBOSaver * _fboSaver;

signals:
    void textureReady(const QImage & image, const QSize & size);
    void contentToSaveRendered(const QImage & fboContent, const QRect & saveArea, const qreal & angle);
    void needToRedraw();
    void initialized();

public slots:
    virtual void renderNext() final;
    virtual void shutDown() final;
};

#endif // RENDERTHREAD_H
