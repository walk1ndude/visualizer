#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

class OpenGLItem : public QQuickItem, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    OpenGLItem();
    ~OpenGLItem();

protected:
    QOpenGLContext * _context;

    void initialize();

protected slots:
    void render();
    void renderNow();

    void sync();

    void cleanup();
    void handleWindowChanged(QQuickWindow * window);
};

#endif // OPENGLITEM_H
