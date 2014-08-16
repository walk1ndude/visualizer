#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/HeadModel.h"
#include "Model/PointsModel.h"

namespace Scene {
    ModelScene::ModelScene(Viewport::ViewportArray ** viewPortArray) :
        AbstractScene(viewPortArray) {

    }

    ModelScene::~ModelScene() {

    }

    void ModelScene::initScene() {
        initMaterials();
        initLightSources();
    }

    bool ModelScene::isEmpty() {
        return _models.empty();
    }

    QVector3D ModelScene::rotation() {
        return _rotation;
    }

    QRect ModelScene::screenSaveRect() {
        return QRect(_screenSaveRect.x(), _screenSaveRect.y(), _screenSaveRect.width(), _screenSaveRect.height());
    }

    void ModelScene::setRotation(const QVector3D & rotation) {
        _rotation = rotation;
        if (_selectedModel) {
            _selectedModel->rotate(rotation);
        }
    }

    void ModelScene::setMouseRotation(const QPointF & startPos, const QPointF & finishPos) {
        if (!viewportArray()->canRotate(startPos, finishPos)) {
            // mouse is not in right viewport, do nothing
            return;
        }

        if (_selectedModel) {
            _selectedModel->rotate(QVector3D((finishPos.y() - startPos.y()) / 20.0f, (finishPos.x() - startPos.x()) / 20.0f, 0.0f));
        }
    }

    void ModelScene::setZoomFactor(const qreal & zoomFactor) {
        viewportArray()->zoom(zoomFactor);
    }

    void ModelScene::setXRange(const ModelInfo::ViewAxisRange & xRange) {
        if (Model::AbstractModelWithPoints * model = dynamic_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(xRange, ModelInfo::XAXIS);
        }
    }

    void ModelScene::setYRange(const ModelInfo::ViewAxisRange & yRange) {
        if (Model::AbstractModelWithPoints * model = dynamic_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(yRange, ModelInfo::YAXIS);
        }
    }

    void ModelScene::setZRange(const ModelInfo::ViewAxisRange & zRange) {
        if (Model::AbstractModelWithPoints * model = dynamic_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->setViewAxisRange(zRange, ModelInfo::ZAXIS);
        }
    }

    void ModelScene::renderScene(const QSize & surfaceSize) {
        viewportArray()->resize(surfaceSize);

        QListIterator<Model::AbstractModel *> modelIterator (_models);
        viewportArray()->render(modelIterator);

        /* some children, like pointsmodel can change its values after rendering -
        * for example depth buffer check affects values of points (z-coordinate)
        */
        if (viewportArray()->postProcess(modelIterator)) {
            emit redraw();
        }
    }

    void ModelScene::cleanUp() {
        qDeleteAll(_textures.begin(), _textures.end());
        _textures.clear();

        // it's just map to tex info, so no need to del anything allocated
        _texturesInModel.clear();
        _texturesInfo.clear();

        _selectedModel = nullptr;
        _selectedTexture = nullptr;

        qDeleteAll(_models.begin(), _models.end());
        _models.clear();

        qDeleteAll(_materials.begin(), _materials.end());
        _materials.clear();

        qDeleteAll(_lightSources.begin(), _lightSources.end());
        _lightSources.clear();
    }

    // need some good defaults here
    void ModelScene::initMaterials() {
        addMaterial(MaterialInfo::Emissive(0.6, 0.6, 0.6, 1.0),
                    MaterialInfo::Diffuse(0.7, 0.8, 0.6, 1.0),
                    MaterialInfo::Specular(0.01, 0.02, 0.02, 0.02),
                    MaterialInfo::Shininess(0.0001));
    }

    void ModelScene::initLightSources() {
        addLightSource(LightInfo::Position(10.0, -10.0, 10.0, 1.0),
                       LightInfo::Color(0.8, 0.8, 0.8, 0.6),
                       LightInfo::AmbientIntensity((GLfloat) 0.002));
    }

    void ModelScene::addMaterial(const MaterialInfo::Emissive & emissive,
                                    const MaterialInfo::Diffuse & diffuse,
                                    const MaterialInfo::Specular & specular,
                                    const MaterialInfo::Shininess & shininess) {
        _materials.append(new MaterialInfo::Material(emissive, diffuse, specular, shininess));
    }

    void ModelScene::addLightSource(const LightInfo::Position & position,
                                       const LightInfo::Color & color,
                                       const LightInfo::AmbientIntensity & ambientIntensity) {
        _lightSources.append(new LightInfo::LightSource(position, color, ambientIntensity));
    }

    void ModelScene::addTexture(TextureInfo::Texture & textureInfo) {
        textureInfo.pixelFormat = QOpenGLTexture::Red;

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
        } else if (Model::AbstractModelWithPoints * model = dynamic_cast<Model::AbstractModelWithPoints *>(_selectedModel)) {
            model->addPoint(point.name, new PointsInfo::ModelPoint(QVector3D(point.position), point.color, 0));
        }
    }

    void ModelScene::addStlModel(ModelInfo::BuffersVN buffers) {
        Model::PointsModel * pointsInModel = new Model::PointsModel;
        _models.append(pointsInModel);

        Model::StlModel * model = new Model::StlModel(pointsInModel);

        // select newly created model
        selectModel(model);

        model->fillBuffers<ModelInfo::BuffersVN>(buffers);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariablesNames() << "lightSource.position" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariablesNames() << "stlMaterial.emissive" << "stlMaterial.diffuse" <<
                           "stlMaterial.specular" << "stlMaterial.shininess");

        model->setViewRange(ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariablesNames() << "ranges.xRange" << "ranges.yRange" << "ranges.zRange");
        
        QObject::connect(model, &Model::StlModel::pointUpdated, this, &Scene::AbstractScene::pointUpdated, Qt::DirectConnection);

        _models.append(model);
    }

    void ModelScene::addHeadModel(SliceInfo::Slices slices) {
        Model::PointsModel * pointsInModel = new Model::PointsModel;
        _models.append(pointsInModel);

        Model::HeadModel * model = new Model::HeadModel(pointsInModel);

        selectModel(model);

        model->init(slices.texture.size);

        model->scale(slices.texture.scaling);

        addTexture(slices.texture);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariablesNames() << "lightSource.position" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariablesNames() << "headMaterial.emissive" << "headMaterial.diffuse" <<
                           "headMaterial.specular" << "headMaterial.shininess");

        model->addTexture(_textures.at(0),
                          ShaderInfo::ShaderVariablesNames() << "texHead");

        model->setViewRange(ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariablesNames() << "ranges.xRange" << "ranges.yRange" << "ranges.zRange");
        
        
        QObject::connect(model, &Model::HeadModel::pointUpdated, this, &Scene::AbstractScene::pointUpdated, Qt::DirectConnection);

        _models.append(model);
    }
}
