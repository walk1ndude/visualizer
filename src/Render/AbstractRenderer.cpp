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
        _selectedScene(nullptr),
        _canRenderContent(false),
        _textureUpdateNeeded(false),
        _contentInitializeNeeded(false),
        _fboRender(nullptr) {

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
            format.setInternalTextureFormat(GL_RGBA16);
            _fboRender = new QOpenGLFramebufferObject(_surfaceSize, format);
        }

        _fboRender->bind();

        render();

        QImage texture = _fboRender->toImage();

        _fboRender->bindDefault();

        if (_takeShot) {
            QRect screenRect = _selectedScene->screenSaveRect();
            // cause this texture is flipped on y axis
            screenRect.setY(0);
            screenRect.setHeight(_surfaceSize.height() / 2);
            qDebug() << screenRect;
            //screenRect.setX(_surfaceSize.width() / 2.0);
            //screenRect.setWidth(_surfaceSize.width() / 2.0);
            emit contentToSaveRendered(_fboRender->toImage(), screenRect, _selectedScene->rotation().y());
        }

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
