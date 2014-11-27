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
        QObject::connect(this, &ModelScene::childrenChanged, [=]() {
            lightSources.clear();
            materials.clear();

            for (QQuickItem * child : childItems()) {
                if (LightInfo::LightSource * lightSource = qobject_cast<LightInfo::LightSource *>(child)) {
                    lightSources.append(lightSource);
                }
                else if (MaterialInfo::Material * material = qobject_cast<MaterialInfo::Material *>(child)) {
                    materials.append(material);
                }
            }

            loaded();
        });
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
        qDeleteAll(_textures.begin(), _textures.end());
        _textures.clear();

        // it's just map to tex info, so no need to del anything allocated
        _texturesInModel.clear();
        _texturesInfo.clear();

        _selectedModel = nullptr;
        _selectedTexture = nullptr;

        materials.clear();
        lightSources.clear();
    }

    QVariant ModelScene::blueprint() const {
        return _blueprint;
    }

    void ModelScene::setBlueprint(const QVariant & blueprint) {
        _blueprint = blueprint;

        emit blueprintChanged(blueprint);
    }

    void ModelScene::addTexture(TextureInfo::Texture & textureInfo) {
        QOpenGLTexture * texture = new QOpenGLTexture(textureInfo.target);

        texture->create();
        texture->setFormat(textureInfo.textureFormat);
        texture->setSize(textureInfo.size.x(), textureInfo.size.y(), textureInfo.size.z());

        texture->allocateStorage();

        texture->setMinMagFilters(QOpenGLTexture::LinearMipMapNearest, QOpenGLTexture::Linear);
        texture->setWrapMode(QOpenGLTexture::ClampToBorder);

        texture->setData(textureInfo.pixelFormat, textureInfo.pixelType,
                         (void *) textureInfo.mergedData.data(), &(textureInfo.pixelTransferOptions));

        texture->generateMipMaps();

        _textures.append(texture);
        _texturesInfo.insert(texture, textureInfo);
        _texturesInModel.insert(_selectedModel, texture);

        textureInfo.mergedData.clear();
    }

    void ModelScene::selectModel(Model::AbstractModel * model) {
        _selectedModel = model;
        emit modelIDChanged(_selectedModel->id());
    }

    void ModelScene::addModel(VolumeInfo::Volume volume) {/*
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

        _models.append(model); */
    }

    Model::AbstractModel *  ModelScene::addModel(const ModelInfo::Model & model) {
        ModelInfo::Params params;

        Model::AbstractModel * modelI = Model::AbstractModel::createModel(model.first, this);

        params = model.second;
        modelI->init(params);

        _models.append(modelI);

        QVariantMap children = params["children"].toMap();

        for (const QString & child : children.keys()) {
            modelI->addChild(
                        addModel(
                            ModelInfo::Model(child, children[child].toMap())
                        )
            );
        }

        return modelI;
    }

    void ModelScene::initScene() {
        QVariantMap modelMap;

        for (const QVariant & model : _blueprint.toList()) {
            modelMap = model.toMap();

            addModel(ModelInfo::Model(
                modelMap["type"].value<ModelInfo::Type>(),
                modelMap["params"].value<ModelInfo::Params>()
                )
            );
        }
    }

    void ModelScene::recieve(const Message::SettingsMessage & message) {
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
