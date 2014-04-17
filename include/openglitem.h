#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    OpenGLItem();
    virtual ~OpenGLItem();

protected:
    QOpenGLContext * _context;

    bool _needsInitialize;

    bool _isTextureUpdated;

    bool _needToDestroyTextures;

    virtual void initialize();
    virtual void initializeTextures();

    virtual void cleanupTextures();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    QOpenGLFramebufferObject * _fbo;

protected slots:
    virtual void render();
    virtual void sync();
    virtual void cleanup();

public slots:
    virtual void destroyContext();

signals:
    void initialized();
};

#endif // OPENGLITEM_H
