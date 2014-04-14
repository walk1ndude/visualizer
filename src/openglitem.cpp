#include <QtQuick/QQuickWindow>

#include <QtCore/QDebug>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _context(0),
    _fbo(0),
    _needsInitialize(false) {

    setFlag(QQuickItem::ItemHasContents);

}

OpenGLItem::~OpenGLItem() {

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

        if (_fbo) {
            delete _fbo;
            _fbo = 0;
        }

        const qreal retinaScale = window()->devicePixelRatio();

        QOpenGLFramebufferObjectFormat formatFBO;

        formatFBO.setSamples(16);
        formatFBO.setMipmap(true);
        formatFBO.setInternalTextureFormat(GL_RGBA);

        _fbo = new QOpenGLFramebufferObject(QSize(width() * retinaScale, height() * retinaScale), formatFBO);

        _context->makeCurrent(window());

        texNode->setTexture(window()->createTextureFromId(_fbo->texture(), _fbo->size()));
        texNode->setRect(boundingRect());

        _fbo->bind();

        render();

        _fbo->bindDefault();
        _fbo->release();

        _fbo->toImage().save("name.png","png");

        _context->swapBuffers(window());
    }
    else {
        texNode->setTexture(window()->createTextureFromId(0, QSize(0, 0)));
    }

    return texNode;
}

void OpenGLItem::contextSwitch() {
    if (_needsInitialize) {

        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setSamples(16);
        format.setVersion(3, 3);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);

        _context->moveToThread(thread());

        _context->setFormat(format);
        _context->create();

        _context->makeCurrent(window());

        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLItem::cleanup, Qt::DirectConnection);

        initializeOpenGLFunctions();
        initialize();
    }

    if (_context) {

        QOpenGLContext * savedContext = QOpenGLContext::currentContext();

        _context->makeCurrent(window());

        //render();

        _context->swapBuffers(window());

        savedContext->makeCurrent(window());
    }
}

void OpenGLItem::initialize() {

}

void OpenGLItem::render() {

}

void OpenGLItem::sync() {

}

void OpenGLItem::cleanup() {

}
