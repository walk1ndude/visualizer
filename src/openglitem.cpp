#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _needsInitialize(false),
    _isTextureUpdated(true),
    _context(0),
    _fbo(0) {

    setFlag(QQuickItem::ItemHasContents);
    QObject::connect(this, &OpenGLItem::windowChanged, this, &OpenGLItem::handleWindowChanged, Qt::DirectConnection);
}

void OpenGLItem::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        window->setClearBeforeRendering(false);
    }
}

OpenGLItem::~OpenGLItem() {
    if (_fbo) {
        delete _fbo;
    }
}

QSGNode * OpenGLItem::updatePaintNode(QSGNode * node, UpdatePaintNodeData * ) {
    QOpenGLContext * savedContext = window()->openglContext();

    if (_needsInitialize) {
        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setSamples(16);
        format.setRedBufferSize(8);
        format.setGreenBufferSize(8);
        format.setBlueBufferSize(8);
        format.setDepthBufferSize(16);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSwapBehavior(QSurfaceFormat::TripleBuffer);

        _context->setFormat(format);
        _context->create();

        _context->makeCurrent(window());

        QObject::connect(window(), SIGNAL(closing(QQuickCloseEvent *)), _context, SIGNAL(aboutToBeDestroyed()));
        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, [=]() { this->cleaningUp(); });

        initializeOpenGLFunctions();
        initialize();

        _needsInitialize = false;

        emit initialized();

        savedContext->makeCurrent(window());
    }

    QSGSimpleTextureNode * texNode = static_cast<QSGSimpleTextureNode *>(node);

    if (!texNode) {
        texNode = new QSGSimpleTextureNode;
    }

    if (_context) {
        const GLsizei viewportWidth = width() * window()->devicePixelRatio();
        const GLsizei viewportHeight = height() * window()->devicePixelRatio();

        if (_fbo) {
            delete _fbo;
            _fbo = 0;
        }

        _fbo = new QOpenGLFramebufferObject(viewportWidth, viewportHeight);
        _fbo->bind();

        _context->makeCurrent(window());

        if (!_isTextureUpdated) {
            initializeTextures();
            _isTextureUpdated = true;
        }

        render();

        savedContext->makeCurrent(window());

        _fbo->bindDefault();

        texNode->setTexture(window()->createTextureFromId(_fbo->texture(), _fbo->size()));
        texNode->setRect(boundingRect());
    }
    else {
        texNode->setTexture(window()->createTextureFromId(0, QSize(0, 0)));
    }

    return texNode;
}

void OpenGLItem::initialize() {

}

void OpenGLItem::initializeTextures() {

}

void OpenGLItem::render() {

}

void OpenGLItem::cleanup() {

}

void OpenGLItem::cleaningUp() {
    QOffscreenSurface cleanUpHelper;
    cleanUpHelper.create();

    QOpenGLContext * savedContext = window()->openglContext();

    _context->makeCurrent(&cleanUpHelper);

    cleanup();

    savedContext->makeCurrent(&cleanUpHelper);
}
