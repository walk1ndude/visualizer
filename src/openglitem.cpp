#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include <QtGui/QOpenGLFramebufferObject>

#include "openglitem.h"

OpenGLItem::OpenGLItem() :
    _context(0),
    _contextQt(0),
    _needsInitialize(false),
    _fbo(0),
    _surface(0) {

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
    _contextQt = window()->openglContext();

    if (_needsInitialize) {

        _context = new QOpenGLContext;

        QSurfaceFormat format;
        format.setVersion(3, 3);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);

        _context->setFormat(format);
        _context->create();

        QObject::connect(_context, &QOpenGLContext::aboutToBeDestroyed, this, &OpenGLItem::cleanup, Qt::DirectConnection);

        _surface = new QOffscreenSurface;
        _surface->setFormat(format);
        _surface->create();

        _fboFormat.setTextureTarget(GL_TEXTURE_2D);
        _fboFormat.setInternalTextureFormat(GL_RGBA);
        _fboFormat.setMipmap(true);

        _contextQt->doneCurrent();
        _context->makeCurrent(_surface);

        initializeOpenGLFunctions();
        initialize();

        _needsInitialize = false;

        emit initialized();
    }

    QSGSimpleTextureNode * texNode = static_cast<QSGSimpleTextureNode *>(node);

    if (!texNode) {
        texNode = new QSGSimpleTextureNode;
    }

    if (_context) {
        const qreal retinaScale = window()->devicePixelRatio();

        const GLsizei viewportWidth = width() * retinaScale;
        const GLsizei viewportHeight = height() * retinaScale;

        if (_fbo) {
            delete _fbo;
            _fbo = 0;
        }

        _fbo = new QOpenGLFramebufferObject(QSize(viewportWidth, viewportHeight), _fboFormat);

        texNode->setTexture(window()->createTextureFromId(_fbo->texture(), _fbo->size()));
        texNode->setRect(boundingRect());

        _fbo->bind();

        _contextQt->doneCurrent();
        _context->makeCurrent(_surface);

        render(viewportWidth, viewportHeight);

        glFlush();
        _context->swapBuffers(_surface);

        _context->doneCurrent();
        _contextQt->makeCurrent(window());

        _contextQt->swapBuffers(window());

        _fbo->bindDefault();
    }
    else {
        texNode->setTexture(window()->createTextureFromId(0, QSize(0, 0)));
    }

    return texNode;
}

void OpenGLItem::initialize() {

}

void OpenGLItem::render(const GLsizei viewportWidth, const GLsizei viewportHeight) {
    Q_UNUSED(viewportWidth);
    Q_UNUSED(viewportHeight);
}

void OpenGLItem::sync() {

}

void OpenGLItem::cleanup() {

}
