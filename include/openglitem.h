#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QSGSimpleTextureNode>

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    OpenGLItem();
    ~OpenGLItem();

protected:
    QOpenGLContext * _context;

    QOpenGLFramebufferObject * _fbo;

    bool _needsInitialize;

    virtual void initialize();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

    void contextSwitch();

protected slots:
    virtual void render();

    virtual void sync();

    virtual void cleanup();
};

#endif // OPENGLITEM_H
