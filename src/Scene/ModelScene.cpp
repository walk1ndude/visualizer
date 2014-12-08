#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/VolumeModel.h"
#include "Model/PointsModel.h"
#include "Model/EvaluatorModel.h"
#include "Model/AxesModel.h"

namespace Scene {
    ModelScene::ModelScene() :
        AbstractScene() {
    }

    ModelScene::~ModelScene() {

    }

    Viewport::ViewportArray * ModelScene::viewportArray() const {
        return _viewportArray;
    }

    Model::AbstractModel * ModelScene::selectedModel() const {
        return qobject_cast<Model::AbstractModel *>(_models.selectedObject());
    }

    void ModelScene::setViewportArray(Viewport::ViewportArray * viewportArray) {
        _viewportArray = viewportArray;

        emit viewportArrayChanged();
    }

    void ModelScene::updateScene() {
        while (!_blueprints.isEmpty()) {
            unpackBlueprint(_blueprints.dequeue());
        }

        for (Model::AbstractModel * model : _models.list()) {
            if (model->updateNeeded()) {
                model->update();
            }
        }
    }

    void ModelScene::renderScene(const QSize & surfaceSize) {
        viewportArray()->resize(surfaceSize);

        updateScene();

        QListIterator<Model::AbstractModel *> modelIterator (_models.list());
        QListIterator<Viewport::Viewport *> viewportIterator (_viewportArray->array());

        render(modelIterator, viewportIterator);

        /* some children, like pointsmodel can change its values after rendering -
        * for example depth buffer check affects values of points (z-coordinate)
        */
        if (postProcess(modelIterator, viewportIterator)) {
            emit redraw();
        }
    }

    void ModelScene::render(QListIterator<Model::AbstractModel *> & modelIterator,
                            QListIterator<Viewport::Viewport *> & viewportIterator) {
        Viewport::ViewportRect boundingRect;
        const Viewport::Viewport * viewport;

        while (viewportIterator.hasNext()) {
            viewport = viewportIterator.next();

            boundingRect = viewport->boundingRect();
            glViewport(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());

            // draw each model
            while (modelIterator.hasNext()) {
                modelIterator.next()->drawModel(viewport);
            }

            modelIterator.toFront();
        }

        viewportIterator.toFront();
    }

    bool ModelScene::postProcess(QListIterator<Model::AbstractModel *> & modelIterator,
                                 QListIterator<Viewport::Viewport *> & viewportIterator) {
        bool redraw = false;

        while (modelIterator.hasNext()) {
            Model::AbstractModel * model = modelIterator.next();

            while (viewportIterator.hasNext()) {
                redraw |= model->checkDepthBuffer(viewportIterator.next());
            }

            viewportIterator.toFront();

            if (redraw) {
                model->update();
            }
        }

        modelIterator.toFront();

        return redraw;
    }

    void ModelScene::cleanUp() {
        AbstractScene::cleanUp();
    }

    QVariant ModelScene::blueprint() const {
        return _blueprint;
    }

    void ModelScene::setBlueprint(const QVariant & blueprint) {
        _blueprints.enqueue(blueprint.value<Blueprint>());

        _blueprint = blueprint;

        emit blueprintChanged(blueprint);
    }

    void ModelScene::selectModel(Model::AbstractModel * model) {
        _models.selectObject(model);

        Message::SettingsMessage message(
                    Message::Sender("scene"),
                    Message::Reciever("sidebar")
                    );

        message.data["modelID"] = QVariant::fromValue(_models.selectedObject()->id());

        emit post(message);
    }

    Model::AbstractModel *  ModelScene::addModel(const ModelInfo::Model & model) {
        ModelInfo::Params params;

        Model::AbstractModel * modelI = Model::AbstractModel::createModel(model.first, this);

        params = model.second;
        modelI->init(params);

        _models.append(modelI);

        QVariantList children = params["children"].toList();

        QVariantMap childsMap;

        for (const QVariant & child : children) {
            childsMap = child.toMap();

            modelI->addChild(addModel(ModelInfo::Model(
                                     childsMap["type"].value<ModelInfo::Type>(),
                                     childsMap["params"].value<ModelInfo::Params>()
                                    )
                                )
            );
        }

        QObject::connect(modelI, &Model::AbstractModel::post, this, &ModelScene::post, Qt::DirectConnection);

        return modelI;
    }

    void ModelScene::initScene() {
        unpackBlueprint(_blueprints.dequeue(), true);
    }

    void ModelScene::unpackBlueprint(const Blueprint & blueprint, const bool & resetScene) {
        if (resetScene) {
            cleanUp();
        }

        QVariantMap helper;

        for (const QVariant & lightSource : blueprint["lightSources"].toList()) {
            lightSources.append(new LightSource(lightSource.toMap()));
        }

        for (const QVariant & material : blueprint["materials"].toList()) {
            materials.append(new Material(material.toMap()));
        }

        for (const QVariant & texture : blueprint["textures"].toList()) {
            textures.append(new Texture(texture.toMap()));
        }

        for (const QVariant & model : blueprint["models"].toList()) {
            helper = model.toMap();

            selectModel(
                        addModel(ModelInfo::Model(
                                 helper["type"].value<ModelInfo::Type>(),
                                 helper["params"].value<ModelInfo::Params>()
                        )
                    )
            );
        }
    }

    void ModelScene::recieve(const Message::SettingsMessage & message) {
        if (message.reciever().startsWith("Scene")) {
            _blueprints.enqueue(message.data["blueprint"].value<Blueprint>());
            return;
        }

        Model::AbstractModel * model = _models[message.reciever()];

        if (model) {
            model->invoke(
                    message.data["action"].toString(),
                    message.data["params"].value<ModelInfo::Params>()
            );
        }
    }
}
