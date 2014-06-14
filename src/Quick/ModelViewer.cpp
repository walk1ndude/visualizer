#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGSimpleTextureNode>

#include "Quick/ModelViewer.h"

#define FBO_WIDTH 768
#define FBO_HEIGHT 768

class TextureNode : public QObject, public QSGSimpleTextureNode {
    Q_OBJECT
public:
    TextureNode(QQuickWindow * window) :
        _size(0, 0),
        _texture(0),
        _window(window) {

        // Our texture node must have a texture, so use the default 0 texture.
        _texture = _window->createTextureFromId(0, QSize(1, 1));
        setTexture(_texture);
        setFiltering(QSGTexture::Linear);
    }

    ~TextureNode() {
            delete _texture;
    }

signals:
    void pendingNewTexture();

public slots:
    // This function gets called on the FBO rendering thread and will store the
    // texture id and size and schedule an update on the window.
    void newTexture(const QImage & image, const QSize & size) {
        _textureMutex.lock();
        _image = image;
        _size = size;
        _textureMutex.unlock();

        // We cannot call QQuickWindow::update directly here, as this is only allowed
        // from the rendering thread or GUI thread.
        emit pendingNewTexture();
    }

    void prepareNode() {
        _textureMutex.lock();
        delete _texture;
        _texture = _window->createTextureFromImage(_image);
        setTexture(_texture);
        _textureMutex.unlock();
    }

private:
    QSize _size;

    QImage _image;

    QMutex _textureMutex;

    QSGTexture *_texture;
    QQuickWindow *_window;
};

namespace Quick {
    ModelViewer::ModelViewer() :
        _modelRenderer(0),
        _takeShot(false) {

        setFlag(QQuickItem::ItemHasContents);

        addModelScene();
    }

    ModelViewer::~ModelViewer() {
        if (_modelRenderer->isRunning()) {
            _modelRenderer->deleteLater();
        }
    }

    bool ModelViewer::takeShot() {
        return _takeShot;
    }

    void ModelViewer::setTakeShot(const bool & takeShot) {
        _takeShot = takeShot;
        emit takeShotChanged(_takeShot);
    }

    QVector3D ModelViewer::rotation() {
        return _rotation;
    }

    void ModelViewer::setRotation(const QVector3D & rotation) {
        _rotation = rotation;
        emit rotationChanged(_rotation);
    }

    qreal ModelViewer::zoomFactor() {
        return _zoomFactor;
    }

    void ModelViewer::setZoomFactor(const qreal & zoomFactor) {
        _zoomFactor = zoomFactor;
        emit zoomFactorChanged(_zoomFactor);
    }

    QVector2D ModelViewer::sRange() {
        return _sRange;
    }

    void ModelViewer::setSRange(const QVector2D & sRange) {
        _sRange = sRange;
        emit sRangeChanged(_sRange);
    }

    QVector2D ModelViewer::tRange() {
        return _tRange;
    }

    void ModelViewer::setTRange(const QVector2D & tRange) {
        _tRange = tRange;
        emit tRangeChanged(_tRange);
    }

    QVector2D ModelViewer::pRange() {
        return _pRange;
    }

    void ModelViewer::setPRange(const QVector2D & pRange) {
        _pRange = pRange;
        emit pRangeChanged(_pRange);
    }

    QVector2D ModelViewer::huRange() {
        return _huRange;
    }

    void ModelViewer::sethuRange(const QVector2D & huRange) {
        _huRange = huRange;
        emit huRangeChanged();
    }

    int ModelViewer::minHU() {
        return (int) _minHU;
    }

    void ModelViewer::setMinHU(const int & minHU) {
        _minHU = minHU;
        emit minHUChanged(_minHU);
    }

    int ModelViewer::maxHU() {
        return _maxHU;
    }

    void ModelViewer::setMaxHU(const int & maxHU) {
        _maxHU = maxHU;
        emit maxHUChanged(_maxHU);
    }

    void ModelViewer::addModelScene() {
        _scenes.push_back(new Scene::ModelScene);
    }

    QSGNode * ModelViewer::updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData *) {
        TextureNode * node = static_cast<TextureNode *>(oldNode);

        if (!_modelRenderer) {
            QOpenGLContext * current = window()->openglContext();
            current->doneCurrent();

            _modelRenderer = new Render::ModelRenderer(current, QSize(FBO_WIDTH, FBO_HEIGHT));
            _modelRenderer->selectScene(_scenes.at(0));

            current->makeCurrent(window());

            QObject::connect(this, &ModelViewer::slicesProcessed, _modelRenderer, &Render::ModelRenderer::drawSlices, Qt::DirectConnection);
            QObject::connect(this, &ModelViewer::modelRead, _modelRenderer, &Render::ModelRenderer::addStlModel);

            //QObject::connect(this, &ModelViewer::rotationChanged, _modelRenderer, &Render::ModelRenderer::setRotation, Qt::DirectConnection);
            QObject::connect(this, &ModelViewer::takeShotChanged, _modelRenderer, &Render::ModelRenderer::setTakeShot, Qt::DirectConnection);
            //QObject::connect(this, &ModelViewer::zoomFactorChanged, _modelRenderer, &Render::ModelRenderer::setZoomFactor, Qt::DirectConnection);

            //QObject::connect(this, &ModelViewer::sRangeChanged, _modelRenderer, &Render::ModelRenderer::setSRange, Qt::DirectConnection);
            //QObject::connect(this, &ModelViewer::tRangeChanged, _modelRenderer, &Render::ModelRenderer::setTRange, Qt::DirectConnection);
            //QObject::connect(this, &ModelViewer::pRangeChanged, _modelRenderer, &Render::ModelRenderer::setPRange, Qt::DirectConnection);

            QObject::connect(window(), &QQuickWindow::sceneGraphInvalidated, _modelRenderer, &Render::ModelRenderer::shutDown);

            QObject::connect(_modelRenderer, &Render::ModelRenderer::appearedSmthToDraw, this, &ModelViewer::appearedSmthToDraw);

            _modelRenderer->moveToThread(_modelRenderer);
            _modelRenderer->start();
        }

        if (!node) {
            node = new TextureNode(window());

            /* Set up connections to get the production of FBO textures in sync with vsync on the
             * rendering thread.
             *
             * When a new texture is ready on the rendering thread, we use a direct connection to
             * the texture node to let it know a new texture can be used. The node will then
             * emit pendingNewTexture which we bind to QQuickWindow::update to schedule a redraw.
             *
             * When the scene graph starts rendering the next frame, the prepareNode() function
             * is used to update the node with the new texture. Once it completes, it emits
             * textureInUse() which we connect to the FBO rendering thread's renderNext() to have
             * it start producing content into its current "back buffer".
             *
             * This FBO rendering pipeline is throttled by vsync on the scene graph rendering thread.
             */
            QObject::connect(_modelRenderer, &Render::ModelRenderer::textureReady, node, &TextureNode::newTexture, Qt::DirectConnection);
            QObject::connect(node, &TextureNode::pendingNewTexture, window(), &QQuickWindow::update);
            QObject::connect(window(), &QQuickWindow::beforeRendering, node, &TextureNode::prepareNode, Qt::DirectConnection);
        }

        // Get the production of FBO textures started..
        QMetaObject::invokeMethod(_modelRenderer, "renderNext");

        node->setRect(boundingRect());

        return node;
    }

    void ModelViewer::drawSlices(SliceInfo::SliceSettings sliceSettings) {
        if (sliceSettings.huRange.size()) {
            _huRange = QVector2D(sliceSettings.huRange[0], sliceSettings.huRange[1]);
            emit huRangeChanged();
        }

        emit slicesProcessed(sliceSettings);
    }

}

#include "ModelViewer.moc"
