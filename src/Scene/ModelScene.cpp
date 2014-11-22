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
            _lightSources.clear();
            _materials.clear();

            for (QQuickItem * child : childItems()) {
                if (LightInfo::LightSource * lightSource = qobject_cast<LightInfo::LightSource *>(child)) {
                    _lightSources.append(lightSource);
                }
                else if (MaterialInfo::Material * material = qobject_cast<MaterialInfo::Material *>(child)) {
                    _materials.append(material);
                }
            }
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

    void ModelScene::setRotation(const QVector3D & rotation) {
        _rotation = rotation;

        QListIterator<Model::AbstractModel *> modelIterator (_models.array());
        Model::AbstractModel * model;
        while (modelIterator.hasNext()) {
            model = modelIterator.next();
            if (Model::EvaluatorModel * evModel = qobject_cast<Model::EvaluatorModel *>(model)) {
                continue;
            }
            if (Model::PointsModel * ptModel = qobject_cast<Model::PointsModel *>(model)) {
                continue;
            }
            model->rotate(rotation);
        }
        /*
        if (_selectedModel) {
            _selectedModel->rotate(rotation);
        }*/
    }

    void ModelScene::setXRange(const ViewRangeInfo::ViewAxisRange & xRange) {
        if (Model::AbstractModelWithPoints * model = qobject_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(xRange, ViewRangeInfo::XAXIS);
        }
    }

    void ModelScene::setYRange(const ViewRangeInfo::ViewAxisRange & yRange) {
        if (Model::AbstractModelWithPoints * model = qobject_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(yRange, ViewRangeInfo::YAXIS);
        }
    }

    void ModelScene::setZRange(const ViewRangeInfo::ViewAxisRange & zRange) {
        if (Model::AbstractModelWithPoints * model = qobject_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(zRange, ViewRangeInfo::ZAXIS);
        }
    }

    void ModelScene::setHuRange(const VolumeInfo::HuRange & huRange) {
        qDebug() << "here";
        if (Model::VolumeModel * model = qobject_cast<Model::VolumeModel *>(_selectedModel)) {
            model->setHuRange(huRange);
        }
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

        _materials.clear();
        _lightSources.clear();
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

    void ModelScene::addPoint(const PointsInfo::Point & point) {
        if (!_selectedModel) {
            return;
        } else {
            _selectedModel->addPoint(point.name,
                                     new PointsInfo::ModelPoint(
                                         PointsInfo::Position3D(point.position),
                                         point.color, point.viewport, point.groups
                                         )
                                     );
        }
    }

    void ModelScene::togglePoint(const QString & point) {
        if (!_selectedModel) {
            return;
        }
        else {
            _selectedModel->hidePoint(point);
        }
    }

    void ModelScene::addModel(ModelInfo::BuffersVN buffers) {
        Model::AbstractModel * pointsInModel = Model::AbstractModel::createModel("PointsModel",
                    Model::Params() = {
                        { "scene", QVariant::fromValue(this) }
                    }
                );

        _models.append(pointsInModel);

        Model::StlModel * model = new Model::StlModel(this);

        model->setPointsModel(qobject_cast<Model::PointsModel *>(pointsInModel));

        selectModel(model);

        model->init(buffers);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariablesNames() << "lightSource.position" << "lightSource.color" <<
                              "lightSource.ambientIntensity" << "lightSource.attenuation");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariablesNames() << "material.emissive" << "material.diffuse" <<
                           "material.specular" << "material.shininess");

        model->setViewRange(ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariablesNames() << "ranges.xRange" << "ranges.yRange" << "ranges.zRange");
        
        QObject::connect(model, &Model::StlModel::pointUpdated, this, &Scene::ModelScene::pointUpdated, Qt::DirectConnection);

        _models.append(model);
    }

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
        
        QObject::connect(model, &Model::VolumeModel::pointUpdated, this, &Scene::ModelScene::pointUpdated, Qt::DirectConnection);

        _models.append(model);
    }

    void ModelScene::addModel(const Model::Type & name,
                              const Model::Params & initParams,
                              const Model::RequestedChildren & children) {
        Model::AbstractModel * model = Model::AbstractModel::createModel(name,
                    Model::Params() = {
                        { "scene", QVariant::fromValue(this) }
                    }
                );

        QHashIterator<Model::Type, Model::Count> it(children);

        while (it.hasNext()) {
            it.next();

            for (int i = 0; i != it.value(); ++ i) {
                model->addChild(Model::AbstractModel::createModel(it.key(),
                    Model::Params() = {
                        { "scene", QVariant::fromValue(this) }
                    }
                ));
            }
        }

        model->init(initParams);

        _models.append(model);
    }

    void ModelScene::initScene() {
        addModel("EvaluatorModel", Model::Params() = {
            { "width", QVariant::fromValue(10) },
            { "height", QVariant::fromValue(10) },
            { "stepX", QVariant::fromValue(10.0f) },
            { "stepY", QVariant::fromValue(0.0f) },
            { "color", QVariant::fromValue(QVector3D(0.0f, 0.0f, 0.5f)) }
        });

        addModel("AxesModel", Model::Params() = {
            { "axesColor", QVariant::fromValue(QVector<QColor>() = {
                QColor::QColor("red"),
                QColor::QColor("green"),
                QColor::QColor("blue")
            }) },
            { "length", QVariant::fromValue(1.5f) }
        });
    }

    void ModelScene::recieve(const Message::SettingsMessage & message) {
        QVariant id = message["modelID"];
        QVariant method = message["method"];

        if (id != QVariant() && method != QVariant()) {
            QVariant params = message["params"];

            _models[id.toUInt()]->invoke(method.toString(), (params != QVariant()) ?  params.value<Model::Params>() : Model::Params());
        }
    }
}
