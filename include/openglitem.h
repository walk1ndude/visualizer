#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    OpenGLItem();
    ~OpenGLItem();

protected:
    bool _needsInitialize;

    virtual void initialize();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    QOpenGLContext * _context;

    QOpenGLFramebufferObject * _fbo;

protected slots:
    virtual void render(const GLsizei viewportWidth, const GLsizei viewportHeight);
    virtual void sync();
    virtual void cleanup();

signals:
    void initialized();
};

#endif // OPENGLITEM_H
