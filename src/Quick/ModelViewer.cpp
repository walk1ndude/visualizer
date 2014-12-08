#include "Quick/ModelViewer.h"
#include "Quick/TextureNode.h"

namespace Quick {
    ModelViewer::ModelViewer() :
        _modelRenderer(nullptr),
        _viewportArray(nullptr) {

        setFlag(QQuickItem::ItemHasContents);

        QObject::connect(this, &ModelViewer::childrenChanged, [=]() {
            _viewportArray = nullptr;

            //find viewportArray that is the nearest to the viewer, if many are present
            for (QQuickItem * child : childItems()) {
                if (Viewport::ViewportArray * viewportArray = qobject_cast<Viewport::ViewportArray *>(child)) {
                    if (_viewportArray) {
                        if (_viewportArray->z() < viewportArray->z()) {
                            _viewportArray = viewportArray;
                        }
                    }
                    else {
                        _viewportArray = viewportArray;
                    }
                }
            }
        });
    }

    ModelViewer::~ModelViewer() {
        if (_modelRenderer->isRunning()) {
            _modelRenderer->wait();
            _modelRenderer->deleteLater();
        }
    }

    QVariant ModelViewer::message() const {
        return QVariant();
    }

    QSize ModelViewer::fboSize() const {
        return _fboSize;
    }

    void ModelViewer::setFboSize(const QSize & fboSize) {
        _fboSize = fboSize;

        emit fboSizeChanged(fboSize);
    }

    QVariantMap ModelViewer::selectedPoint() const {
        return _selectedPoint;
    }
    
    void ModelViewer::setSelectedPoint(const QVariantMap & selectedPoint) {
        _selectedPoint = selectedPoint;
    }

    Scene::ModelScene * ModelViewer::modelScene() const {
        return _modelScenes.last();
    }

    void ModelViewer::setModelScene(Scene::ModelScene * modelScene) {
        _modelScenes.push_back(modelScene);

        emit modelSceneChanged();
    }
    
    void ModelViewer::updatePoint(const PointsInfo::UpdatedPoint & point) {
        QVariantMap map;

        //map["name"] = point.name;
        map["position"] = point.position;
        map["modelID"] = point.modelId();

        emit pointUpdated(map);
    }

    Viewport::ViewportArray * ModelViewer::viewportArray() const {
        return _viewportArray;
    }

    void ModelViewer::setViewportArray(Viewport::ViewportArray * viewPortArray) {
        _viewportArray = viewPortArray;
    }

    QSGNode * ModelViewer::updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * data) {
        Q_UNUSED(data)

        TextureNode * node = static_cast<TextureNode *>(oldNode);

        if (!_modelRenderer) {
            QOpenGLContext * current = window()->openglContext();
            current->doneCurrent();

            _modelRenderer = new Render::ModelRenderer(current, _fboSize);
            _modelRenderer->selectScene(_modelScenes.last());

            current->makeCurrent(window());

            QObject::connect(window(), &QQuickWindow::sceneGraphInvalidated, _modelRenderer, &Render::ModelRenderer::shutDown);

            QObject::connect(this, &ModelViewer::fboSizeChanged, _modelRenderer, &Render::ModelRenderer::setSurfaceSize);

            QObject::connect(this, (void (ModelViewer::*)(const Message::SettingsMessage &)) &ModelViewer::post,
                             _modelRenderer, (void (Render::ModelRenderer::*)(const Message::SettingsMessage &)) &Render::ModelRenderer::recieve);

            QObject::connect(_modelRenderer, (void (Render::ModelRenderer::*)(const Message::SettingsMessage &)) &Render::ModelRenderer::post,
                             this, (void (ModelViewer::*)(const Message::SettingsMessage &)) &ModelViewer::recieveMessage);

            _modelRenderer->moveToThread(_modelRenderer);
            _modelRenderer->start();

            update();
            return nullptr;
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

    void ModelViewer::recieve(const QVariant & message) {
       if (!message.canConvert<Message::SettingsMessage>()) {
            recieveMessage(Message::SettingsMessage::toMessage(message));
        }
        else {
            recieveMessage(message.value<Message::SettingsMessage>());
        }
    }

    void ModelViewer::recieveMessage(const Message::SettingsMessage & message) {
        // TODO: if reciever empty - this class is final destination
        if (message.reciever().isEmpty()) {
            return;
        }

        if (message.isReliable()) {
            postMessage(message);
        }
    }

    void ModelViewer::postMessage(const Message::SettingsMessage & message) {
        // some messages could have multiple recievers - we need to send to them also,
        // we don't need (or it would have been a hell lot of code) destination: QML of C++
        // so post to both

        for (const Message::Reciever & reciever : message.recievers()) {
            emit post(Message::SettingsMessage::toVariantMap(message, message.sender(), reciever));

            Message::SettingsMessage ms(message.sender(), reciever);
            ms.setReliableTime(message.reliableTime());
            ms.data = message.data;
            emit post(ms);
        }

        emit post(message);
    }
}
