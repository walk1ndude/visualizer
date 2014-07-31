#include <QtGui/QGuiApplication>
#include <QtGui/QImage>

#include <cmath>

#include "Render/AbstractRenderer.h"

namespace Render {
    void FBOSaver::saveToDisk(const QImage & fboContent, const QRect & saveArea, const qreal & angle) {
        fboContent.copy(saveArea).save((angle > 99 ? "" : (angle > 9 ? "0" : "00")) + QString::number(angle * 10) + ".png");
    }

    AbstractRenderer::AbstractRenderer(QOpenGLContext * context, const QSize & surfaceSize) :
        _canRenderContent(false),
        _textureUpdateNeeded(false),
        _contentInitializeNeeded(false),
        _surfaceSize(surfaceSize),
        _takeShot(false),
        _selectedScene(nullptr),
        _fboRender(nullptr),
        _fboDisplay(nullptr) {

        _context = new QOpenGLContext;

        QSurfaceFormat surfaceFormat = context->format();
        _context->setFormat(surfaceFormat);

        _context->setShareContext(context);

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

    void AbstractRenderer::selectScene(Scene::AbstractScene * scene) {
        QMutexLocker locker(&_renderMutex);

        if (!scene->isInitialized()) {
            scene->initScene(_surfaceSize);
        }

        // no more connection with previous scene
        if (_selectedScene) {
            disconnectWithScene(_selectedScene);
        }

        _selectedScene = scene;
        // calling render here is legit 'cause sender and reciever are in the same thread
        connectWithScene(_selectedScene);

        _sceneHistory.insert(scene);
    }

    void AbstractRenderer::connectWithScene(Scene::AbstractScene * scene) {
        QObject::connect(scene, &Scene::AbstractScene::redraw, this, &Render::AbstractRenderer::render);
    }

    void AbstractRenderer::disconnectWithScene(Scene::AbstractScene * scene) {
        QObject::disconnect(scene, &Scene::AbstractScene::redraw, this, &Render::AbstractRenderer::render);
    }

    Scene::AbstractScene * AbstractRenderer::selectedScene() {
        return _selectedScene;
    }

    QSize AbstractRenderer::surfaceSize() {
        return _surfaceSize;
    }

    void AbstractRenderer::renderNext() {
        QMutexLocker locker(&_renderMutex);
        activateContext();

        if (!_fboRender) {
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            format.setInternalTextureFormat(GL_RGBA16);
            _fboRender = new QOpenGLFramebufferObject(_surfaceSize, format);
            _fboDisplay = new QOpenGLFramebufferObject(_surfaceSize, format);
        }

        _fboRender->bind();

        render();

        glFlush();

        _fboRender->bindDefault();
        std::swap(_fboDisplay, _fboRender);

        if (_takeShot) {
            QRect screenRect = _selectedScene->screenSaveRect();
            // cause this texture is flipped on y axis
            screenRect.setY(0);
            screenRect.setHeight(_surfaceSize.height() / 2);
            emit contentToSaveRendered(_fboRender->toImage(), screenRect, _selectedScene->rotation().y());
        }

        emit textureReady(_fboDisplay->texture(), _surfaceSize);
    }

    void AbstractRenderer::shutDown() {
        _context->makeCurrent(_surface);

        if (_fboRender) {
            delete _fboRender;
        }

        if (_fboDisplay) {
            delete _fboDisplay;
        }

        cleanUp();

        delete _context;

        _surface->deleteLater();

        moveToThread(QGuiApplication::instance()->thread());
        exit();
    }

    void AbstractRenderer::cleanUp() {
        QMutexLocker locker(&_renderMutex);

        activateContext();

        QSetIterator<Scene::AbstractScene *> it (_sceneHistory);

        while (it.hasNext()) {
            it.next()->cleanUp();
        }
    }
}
