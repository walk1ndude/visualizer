#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOffscreenSurface>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    OpenGLItem();
    ~OpenGLItem();

protected:
    QOpenGLContext * _context;

    QOffscreenSurface * _surface;

    bool _needsInitialize;

    virtual void initialize();

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

protected slots:
    virtual void render();
    virtual void sync();
    virtual void cleanup();

private slots:
    void windowChangedSettings(QQuickWindow * window);
};

#endif // OPENGLITEM_H
