#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_4_1_Core>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QVector3D>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_4_1_Core {
    Q_OBJECT

    Q_PROPERTY(bool takeShot READ takeShot WRITE setTakeShot NOTIFY takeShotChanged)
public:
    OpenGLItem();
    virtual ~OpenGLItem();

    bool takeShot();
    void setTakeShot(const bool & takeShot);

protected:
    bool _needsInitialize;

    bool _isTextureUpdated;

    QVector3D _rotation;

    QRectF _screenSaveRect;

    virtual void initialize();
    virtual void initializeTextures();
    virtual void initializeViewPorts();

    virtual void render();
    virtual void cleanup();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    QOpenGLContext * _context;
    QOpenGLContext * _savedContext;

    QOpenGLFramebufferObject * _fbo;

    QOffscreenSurface _cleanUpHelper;

    bool _takeShot;
    void cleaningUp();

signals:
    void initialized();
    void takeShotChanged();

private slots:
    void handleWindowChanged(QQuickWindow * window);
};

#endif // OPENGLITEM_H
