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
    QOpenGLContext * _context;
    QOpenGLContext * _contextQt;

    bool _needsInitialize;

    virtual void initialize();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    QOpenGLFramebufferObjectFormat _fboFormat;

    QOpenGLFramebufferObject * _fbo;

    QOffscreenSurface * _surface;

protected slots:
    virtual void render(const GLsizei viewportWidth, const GLsizei viewportHeight);
    virtual void sync();
    virtual void cleanup();

private slots:
    void windowChangedSettings(QQuickWindow * window);

signals:
    void initialized();
};

#endif // OPENGLITEM_H
