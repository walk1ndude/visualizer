#include <QtQuick/QQuickWindow>

#include <QtCore/QDebug>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _program(0),
    _context(0),
    _thread_t(0),
    _isContextInited(false) {

    QObject::connect(this, &OpenGLItem::windowChanged, this, &OpenGLItem::handleWindowChanged);
}

OpenGLItem::~OpenGLItem() {

}

void OpenGLItem::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        QObject::connect(window, &QQuickWindow::beforeRendering, this, &OpenGLItem::renderNow, Qt::DirectConnection);
        QObject::connect(window, &QQuickWindow::beforeSynchronizing, this, &OpenGLItem::sync, Qt::DirectConnection);

        window->setClearBeforeRendering(false);
    }
}

void OpenGLItem::initialize() {

}

void OpenGLItem::renderNow() {
    window()->setHeight(height());
    window()->setWidth(width());

    bool needsInitialize = false;

    QOpenGLContext * savedContext = QOpenGLContext::currentContext();

    if (!_context) {

        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setSamples(16);
        format.setVersion(3, 3);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);

        _context->setFormat(format);
        _context->create();

        needsInitialize = true;
    }

    _context->makeCurrent(window());

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    _context->swapBuffers(window());

    savedContext->makeCurrent(window());
}

void OpenGLItem::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

void OpenGLItem::sync() {

}

void OpenGLItem::cleanup() {
    if (_program) {
        delete _program;
        _program = 0;
    }
}
