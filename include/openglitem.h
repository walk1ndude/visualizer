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
    ~OpenGLItem();

protected:
    bool _needsInitialize;

    virtual void initialize();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    QOpenGLContext * _context;

    QOpenGLFramebufferObject * _fbo;

protected slots:
    virtual void render();
    virtual void sync();
    virtual void cleanup();

    void windowChanged(QQuickWindow * window);

signals:
    void initialized();
};

#endif // OPENGLITEM_H
