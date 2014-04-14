#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <QtGui/QOpenGLFramebufferObject>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _context(0),
    _surface(0),
    _needsInitialize(false) {

    setFlag(QQuickItem::ItemHasContents);

    QObject::connect(this, &QQuickItem::windowChanged, this, &OpenGLItem::windowChangedSettings);
}

OpenGLItem::~OpenGLItem() {

}

void OpenGLItem::windowChangedSettings(QQuickWindow * window) {
    if (window) {
       window->setClearBeforeRendering(false);
    }
}

QSGNode * OpenGLItem::updatePaintNode(QSGNode * node, UpdatePaintNodeData * ) {
    if (_needsInitialize) {

        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setSamples(16);
        format.setVersion(3, 3);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);

        _context->setFormat(format);
        _context->create();

        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLItem::cleanup, Qt::DirectConnection);

        _surface = new QOffscreenSurface;
        _surface->setFormat(format);
        _surface->create();

        _context->makeCurrent(window());

        initializeOpenGLFunctions();
        initialize();

        _needsInitialize = false;
    }

    QSGSimpleTextureNode * texNode = static_cast<QSGSimpleTextureNode *>(node);

    if (!texNode) {
        texNode = new QSGSimpleTextureNode;
    }

    if (_context && width() && height()) {
        const qreal retinaScale = window()->devicePixelRatio();

        QOpenGLFramebufferObject * fbo = new QOpenGLFramebufferObject(width() * retinaScale, height() * retinaScale);

        texNode->setTexture(window()->createTextureFromId(fbo->texture(), fbo->size()));
        texNode->setRect(boundingRect());

        fbo->bind();

        _context->makeCurrent(window());

        render();

        fbo->release();

        _context->swapBuffers(window());

        delete fbo;
    }
    else {
        texNode->setTexture(window()->createTextureFromId(0, QSize(0, 0)));
    }

    return texNode;
}

void OpenGLItem::initialize() {

}

void OpenGLItem::render() {

}

void OpenGLItem::sync() {

}

void OpenGLItem::cleanup() {

}
