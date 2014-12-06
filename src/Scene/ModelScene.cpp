#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/VolumeModel.h"
#include "Model/PointsModel.h"
#include "Model/EvaluatorModel.h"
#include "Model/AxesModel.h"

namespace Scene {
    ModelScene::ModelScene() :
        AbstractScene(),
        _selectedModel(nullptr),
        _selectedTexture(nullptr) {
    }

    ModelScene::~ModelScene() {

    }

    Viewport::ViewportArray * ModelScene::viewportArray() const {
        return _viewportArray;
    }

    Model::AbstractModel * ModelScene::selectedModel() const {
        return _selectedModel;
    }

    void ModelScene::setViewportArray(Viewport::ViewportArray * viewportArray) {
        _viewportArray = viewportArray;

        emit viewportArrayChanged();
    }

    QVector3D ModelScene::rotation() const {
        return _rotation;
    }

    void ModelScene::updateScene() {
        while (!_blueprints.isEmpty()) {
            unpackBlueprint(_blueprints.dequeue());
        }

        for (Model::AbstractModel * model : _models.array()) {
            if (model->updateNeeded()) {
                model->update();
            }
        }
    }

    void ModelScene::renderScene(const QSize & surfaceSize) {
        viewportArray()->resize(surfaceSize);

        updateScene();

        QListIterator<Model::AbstractModel *> modelIterator (_models.array());
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
        _selectedModel = nullptr;
        _selectedTexture = nullptr;

        AbstractScene::cleanUp();
    }

    QVariant ModelScene::blueprint() const {
        return _blueprint;
    }

    void ModelScene::setBlueprint(const QVariant & blueprint) {
        _blueprint = blueprint;

        emit blueprintChanged(blueprint);
    }

    void ModelScene::selectModel(Model::AbstractModel * model) {
        _selectedModel = model;
        emit modelIDChanged(_selectedModel->id());
    }
/*
    void ModelScene::addModel(VolumeInfo::Volume volume) {
        Model::PointsModel * pointsInModel = new Model::PointsModel(this);
        _models.append(pointsInModel);

        Model::VolumeModel * model = new Model::VolumeModel(this, pointsInModel);

        model->setPointsModel(pointsInModel);

        selectModel(model);

        model->init(volume.texture.size, volume.physicalSize, volume.texture.scaling);
        model->scale(volume.texture.scaling);

        addTexture(volume.texture);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariablesNames() << "lightSource.position" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariablesNames() << "material.emissive" << "material.diffuse" <<
                           "material.specular" << "material.shininess");

        model->addTexture(_textures.back(),
                          ShaderInfo::ShaderVariablesNames() << "volume");

        model->setViewRange(ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariablesNames() << "ranges.xRange" << "ranges.yRange" << "ranges.zRange");

        model->setSlope(volume.slope);
        model->setIntercept(volume.intercept);

        model->setWindowCenter(volume.windowCenter);
        model->setWindowWidth(volume.windowWidth);

        model->setHuRange(volume.huRange);
        model->setValueRange(volume.valueRange);

        QObject::connect(model, &Model::AbstractModel::post, this, &Scene::ModelScene::post, Qt::DirectConnection);

        _models.append(model);
    }*/

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

        return modelI;
    }

    void ModelScene::initScene() {
        unpackBlueprint(_blueprint.toMap(), true);
    }

    void ModelScene::unpackBlueprint(const SceneInfo::Blueprint & blueprint, const bool & resetScene) {
        if (resetScene) {
            cleanUp();
        }

        QVariantMap helper;

        for (const QVariant & lightSource : blueprint["lightSources"].toList()) {
            lightSources.append(new LightInfo::LightSource(lightSource.toMap()));
        }

        for (const QVariant & material : blueprint["materials"].toList()) {
            materials.append(new MaterialInfo::Material(material.toMap()));
        }

        for (const QVariant & texture : blueprint["textures"].toList()) {
            textures.append(new TextureInfo::Texture(texture.toMap()));
        }

        for (const QVariant & model : blueprint["models"].toList()) {
            helper = model.toMap();

            addModel(ModelInfo::Model(
                helper["type"].value<ModelInfo::Type>(),
                helper["params"].value<ModelInfo::Params>()
                )
            );
        }
    }

    void ModelScene::recieve(const Message::SettingsMessage & message) {
        if (message.reciever().startsWith("Scene")) {
            _blueprints.enqueue(message.data["blueprint"].value<SceneInfo::Blueprint>());
            return;
        }

        if (message.data.contains("modelID") && message.data.contains("action")) {
            QVariant id = message.data["modelID"];
            QVariant action = message.data["action"];

            _models[id.toUInt()]->invoke(action.toString(),
                                         (message.data.contains("params")) ?
                                             message.data["params"].value<ModelInfo::Params>() : ModelInfo::Params()
                                         );
        }
    }
}
