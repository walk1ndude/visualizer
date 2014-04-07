#include <QtCore/QCoreApplication>

#include <QtGui/QPainter>

#include "openglwindow.h"

OpenGLWindow::OpenGLWindow(const QSurfaceFormat & surfaceFormat, QWindow * parent) :
    QWindow(parent),
    _context(0),
    _updatePending(false),
    _animating(false),
    _device(0),
    _surfaceFormat(surfaceFormat) {

    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(surfaceFormat);
}

OpenGLWindow::~OpenGLWindow() {

}

void OpenGLWindow::render(QPainter * painter) {
    Q_UNUSED(painter);
}

void OpenGLWindow::initialize() {

}

void OpenGLWindow::render() {
    if (!_device) {
        _device = new QOpenGLPaintDevice;
    }

    _openglFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    _device->setSize(size());

    QPainter painter(_device);
    render(&painter);
}

void OpenGLWindow::renderLater() {
    if (!_updatePending) {
        _updatePending = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool OpenGLWindow::event(QEvent * event) {
    switch (event->type()) {
    case QEvent::UpdateRequest:
        _updatePending = false;
        renderNow();
        return false;
    default:
        return QWindow::event(event);
    }
}

void OpenGLWindow::exposeEvent(QExposeEvent * event) {
    Q_UNUSED(event);

    if (isExposed()) {
        renderNow();
    }
}

void OpenGLWindow::resizeEvent(QResizeEvent * event) {
    Q_UNUSED(event);

    if (isExposed()) {
        renderNow();
    }
}

void OpenGLWindow::renderNow() {
    if (!isExposed()) {
        return;
    }

    bool needsInitialize = false;

    if (!_context) {
        _context = new QOpenGLContext(this);
        _context->setFormat(_surfaceFormat);
        _context->create();

        needsInitialize = true;
    }

    _context->makeCurrent(this);

    _openglFuncs = _context->versionFunctions<QOpenGLFunctions_3_3_Core>();

    if (needsInitialize) {
        _openglFuncs->initializeOpenGLFunctions();
        initialize();
    }

    render();

    _context->swapBuffers(this);

    if (_animating) {
        renderLater();
    }
}

void OpenGLWindow::setAnimating(bool animating) {
    _animating = animating;

    if (animating) {
        renderLater();
    }
}
