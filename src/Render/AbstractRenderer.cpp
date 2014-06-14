#include <QtGui/QGuiApplication>
#include <QtGui/QImage>

#include <cmath>

#include "Render/AbstractRenderer.h"

namespace Render {
    void FBOSaver::saveToDisk(const QImage & fboContent, const QRect & saveArea, const qreal & angle) {
        fboContent.copy(saveArea).save((angle > 99 ? "" : (angle > 9 ? "0" : "00")) + QString::number(angle * 10) + ".png");
    }

    AbstractRenderer::AbstractRenderer(QOpenGLContext * context, const QSize & surfaceSize) :
        _surfaceSize(surfaceSize),
        _takeShot(false),
        _canRenderContent(false),
        _textureUpdateNeeded(false),
        _contentInitializeNeeded(false),
        _fboRender(nullptr) {

        _context = new QOpenGLContext;

        QSurfaceFormat surfaceFormat = context->format();
        surfaceFormat.setVersion(4, 1);
        surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
        surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);

        surfaceFormat.setRedBufferSize(8);
        surfaceFormat.setGreenBufferSize(8);
        surfaceFormat.setBlueBufferSize(8);
        surfaceFormat.setDepthBufferSize(8);
        surfaceFormat.setStencilBufferSize(8);

        //_context->setShareContext(context);
        _context->setFormat(surfaceFormat);

        _context->create();
        _context->moveToThread(this);

        _context->doneCurrent();

        _surface = new QOffscreenSurface;
        _surface->setFormat(surfaceFormat);
        _surface->create();

        _fboSaver = new FBOSaver;
        QThread * fboSaverThread = new QThread;

        _fboSaver->moveToThread(fboSaverThread);

        QObject::connect(this, &AbstractRenderer::contentToSaveRendered, _fboSaver, &FBOSaver::saveToDisk);
        QObject::connect(_fboSaver, &FBOSaver::destroyed, fboSaverThread, &QThread::quit);
        QObject::connect(fboSaverThread, &QThread::finished, fboSaverThread, &QThread::deleteLater);

        fboSaverThread->start();

        QObject::connect(this, &AbstractRenderer::needToRedraw, this, &AbstractRenderer::renderNext);
    }

    AbstractRenderer::~AbstractRenderer() {
        delete _fboSaver;
    }

    bool AbstractRenderer::updateContent() {
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

    bool AbstractRenderer::activateContext() {
        return _context->makeCurrent(_surface);
    }

    void AbstractRenderer::setSurface(QOffscreenSurface * surface) {
        _surface = surface;
    }

    void AbstractRenderer::renderNext() {
        QMutexLocker locker (&_renderMutex);
        activateContext();

        if (!_fboRender) {
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            format.setInternalTextureFormat(GL_RGB16);
            _fboRender = new QOpenGLFramebufferObject(_surfaceSize, format);
        }

        _fboRender->bind();

        render();
        glFlush();

        QImage texture = _fboRender->toImage();

        _fboRender->bindDefault();

        if (_takeShot) {/*
            emit contentToSaveRendered(_fboDisplay->toImage(),
                                       QRect(_screenSaveRect.x(), _screenSaveRect.y(), _screenSaveRect.width(), _screenSaveRect.height()),
                                       _rotation.y() + 180.0);*/
        }

        delete _fboRender;
        _fboRender = nullptr;

        emit textureReady(texture, _surfaceSize);
    }

    void AbstractRenderer::shutDown() {
        _context->makeCurrent(_surface);

        if (_fboRender) {
            delete _fboRender;
        }

        cleanUp();

        delete _context;

        _surface->deleteLater();

        exit();
        moveToThread(QGuiApplication::instance()->thread());
    }
}
