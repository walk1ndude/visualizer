#include "renderthread.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QImage>

#include <QtCore/QDebug>

#include <cmath>

void FBOSaver::saveToDisk(const QImage & fboContent, const QRect & saveArea, const qreal & angle) {
    fboContent.copy(saveArea).save((angle > 99 ? "" : (angle > 9 ? "0" : "00")) + QString::number(angle) + ".png");
}

RenderThread::RenderThread(QOpenGLContext * context, const QSize & surfaceSize) :
    _surfaceSize(surfaceSize),
    _rotation(QVector3D(0, 0, 0)),
    _takeShot(true),
    _canRenderContent(false),
    _textureUpdateNeeded(false),
    _contentInitializeNeeded(false),
    _fboRender(0),
    _fboDisplay(0) {

    _context = new QOpenGLContext;

    QSurfaceFormat surfaceFormat = context->format();
    surfaceFormat.setVersion(4, 1);
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

    //_context->setShareContext(context);
    _context->setFormat(surfaceFormat);

    _context->create();
    _context->moveToThread(this);

    _surface = new QOffscreenSurface;
    _surface->setFormat(surfaceFormat);
    _surface->create();

    _fboSaver = new FBOSaver;
    QThread * fboSaverThread = new QThread;

    _fboSaver->moveToThread(fboSaverThread);

    QObject::connect(this, &RenderThread::contentToSaveRendered, _fboSaver, &FBOSaver::saveToDisk);
    QObject::connect(_fboSaver, &FBOSaver::destroyed, fboSaverThread, &QThread::quit);
    QObject::connect(fboSaverThread, &QThread::finished, fboSaverThread, &QThread::deleteLater);

    fboSaverThread->start();

    QObject::connect(this, &RenderThread::needToRedraw, this, &RenderThread::renderNext);
}

RenderThread::~RenderThread() {
    delete _fboSaver;
}

bool RenderThread::updateContent() {
    if (_canRenderContent) {
        _textureUpdateNeeded = true;
        return true;
    }
    else {
        _canRenderContent = true;
        _contentInitializeNeeded = true;
        return false;
    }
}

void RenderThread::setSurface(QOffscreenSurface * surface) {
    _surface = surface;
}

void RenderThread::renderNext() {
    _context->makeCurrent(_surface);

    if (!_fboRender) {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setInternalTextureFormat(GL_RGB16);
        _fboRender = new QOpenGLFramebufferObject(_surfaceSize, format);
        _fboDisplay = new QOpenGLFramebufferObject(_surfaceSize, format);
    }

    _fboRender->bind();

    if (_canRenderContent) {
        if (_contentInitializeNeeded) {
            initialize();
            initializeViewPorts();
            _contentInitializeNeeded = false;

            emit initialized();
        }

        if (_textureUpdateNeeded) {
            updateTextures();
            _textureUpdateNeeded = false;
        }

        render();
    }

    glFlush();

    _fboRender->bindDefault();
    qSwap(_fboRender, _fboDisplay);

    if (_takeShot) {
        if (_rotation.y() - trunc(_rotation.y()) == 0) {
            emit contentToSaveRendered(_fboDisplay->toImage(),
                                       QRect(_screenSaveRect.x(), _screenSaveRect.y(), _screenSaveRect.width(), _screenSaveRect.height()),
                                       _rotation.y() + 180.0);
        }
    }

    emit textureReady(_fboDisplay->toImage(), _surfaceSize);
}

void RenderThread::shutDown() {
    _context->makeCurrent(_surface);

    delete _fboRender;
    delete _fboDisplay;

    cleanUp();

    delete _context;

    _surface->deleteLater();

    exit();
    moveToThread(QGuiApplication::instance()->thread());
}
