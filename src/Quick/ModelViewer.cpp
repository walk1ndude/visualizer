#include "Quick/ModelViewer.h"
#include "Quick/TextureNode.h"

namespace Quick {
    ModelViewer::ModelViewer() :
        _modelRenderer(nullptr),
        _viewportArray(nullptr),
        _modelID(-1) {

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

    void ModelViewer::togglePoint(const QString & point) {
        /*
        recieve("modelViewer", "model 2", "togglePoint",
            ModelInfo::Params() = {
                { "point", QVariant(point) }
        });*/
    }

    int ModelViewer::modelID() const {
        return _modelID;
    }

    void ModelViewer::setModelID(const int & modelID) {
        _modelID = modelID;
        emit modelIDChanged(_modelID);
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

            QObject::connect(_modelRenderer, &Render::ModelRenderer::modelIDChanged, this, &ModelViewer::setModelID, Qt::DirectConnection);

            QObject::connect(this, &ModelViewer::fboSizeChanged, _modelRenderer, &Render::ModelRenderer::setSurfaceSize, Qt::DirectConnection);

            QObject::connect(this, (void (ModelViewer::*)(const Message::SettingsMessage &)) &ModelViewer::post,
                             _modelRenderer, (void (Render::ModelRenderer::*)(const Message::SettingsMessage &)) &Render::ModelRenderer::recieve, Qt::DirectConnection);
            QObject::connect(_modelRenderer, (void (Render::ModelRenderer::*)(const Message::SettingsMessage &)) &Render::ModelRenderer::post,
                             this, (void (ModelViewer::*)(const Message::SettingsMessage &)) &ModelViewer::recieve, Qt::DirectConnection);

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

    void ModelViewer::addPoint(const QPointF & position, Viewport::Viewport * viewport) {
        if (_selectedPoint.isEmpty()) {
            return;
        }

        PointsInfo::Point selectedPoint;

        selectedPoint.position = QPointF(position.x(), position.y());
        selectedPoint.viewport = viewport;

        selectedPoint.name = qvariant_cast<PointsInfo::PointID>(_selectedPoint["name"]);
        selectedPoint.groups = qvariant_cast<PointsInfo::Groups>(_selectedPoint["groups"]);
        selectedPoint.color = qvariant_cast<PointsInfo::Color>(_selectedPoint["color"]);
/*
        recieve("modelViewer", "model 1", "addPoint",
            ModelInfo::Params() = {
                { "point", QVariant::fromValue(selectedPoint) }
        });*/
    }
/*
    void ModelViewer::recieve(const Message::) {
        // TODO: if reciever empty - this class is final destination
        if (reciever.isEmpty()) {
            return;
        }

        Message::SettingsMessage message(sender, reciever);

        if (reciever.startsWith("model ")) {
            QRegExp findID("\\d+$");
            findID.indexIn(reciever);

            uint modelID = findID.capturedTexts().at(0).toUInt();

            message.data["modelID"] = QVariant(modelID);
            message.data["action"] = QVariant(action);
            message.data["params"] = QVariant(params);

            emit post(message);
        }
    }
*/
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
            if (message.reciever().startsWith("sidebar")) {
                if (message.data.contains("point")) {
                    PointsInfo::UpdatedPoint point = message.data["point"].value<PointsInfo::UpdatedPoint>();

                    QVariantMap map;

                    //map["name"] = point.name;
                    map["position"] = point.position;
                    map["modelID"] = point.modelId();

                    emit pointUpdated(map);
                }
            }
            else {
                if (message.data["action"] == "addPoint") {
                    emit post(Message::SettingsMessage::toVariantMap(message, message.sender(), Message::Reciever("viewports")));
                }

                emit post(message);
            }
        }
    }
}
