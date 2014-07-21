#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/HeadModel.h"

namespace Scene {
    ModelScene::ModelScene() :
        AbstractScene() {

    }

    ModelScene::~ModelScene() {

    }

    void ModelScene::initScene(const QSize & surfaceSize) {
        initializeViewPorts(surfaceSize);

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
        _viewPorts.rotate(_rotation.x(), _rotation.y(), _rotation.z());
    }

    void ModelScene::setZoomFactor(const qreal & zoomFactor) {
        _viewPorts.zoom(zoomFactor);
    }

    void ModelScene::setXRange(const ModelInfo::ViewAxisRange & xRange) {
        _selectedModel->setViewAxisRange(xRange, ModelInfo::XAXIS);
    }

    void ModelScene::setYRange(const ModelInfo::ViewAxisRange & yRange) {
        _selectedModel->setViewAxisRange(yRange, ModelInfo::YAXIS);
    }

    void ModelScene::setZRange(const ModelInfo::ViewAxisRange & zRange) {
        _selectedModel->setViewAxisRange(zRange, ModelInfo::ZAXIS);
    }

    void ModelScene::addPoint(const PointsInfo::Point & point) {
        QVector4D rayDirection = _viewPorts.calculateRayDir(point.position);

        qDebug() << rayDirection;

        if (Model::HeadModel * selectedModel = dynamic_cast<Model::HeadModel *>(_selectedModel)) {
            selectedModel->addPoint(point.name, PointsInfo::FacePoint(rayDirection, point.color), point.name);
        }
    }

    void ModelScene::renderScene(const QSize & surfaceSize) {
        _viewPorts.resize(surfaceSize);

        ViewPort::ViewPort viewPort;
        ViewPort::ViewPortRect boundingRect;

        ViewPort::ViewPortsIterator itV (_viewPorts);
        QVectorIterator<Model::AbstractModel *> itM (_models);

        // for each viewport
        while (itV.hasNext()) {
            viewPort = itV.next();

            boundingRect = viewPort.boundingRect();
            if (viewPort.projectionType() == ViewPort::ViewPort::TOP) {
                _screenSaveRect = boundingRect;
            }

            glViewport(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());

            // draw each model
            while (itM.hasNext()) {
                itM.next()->drawModel(viewPort);
            }

            itM.toFront();
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

    void ModelScene::initializeViewPorts(const QSize & surfaceSize) {
        ViewPort::ViewPorts viewPorts;

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0.5, 0.5, 0.5, 0.5), ViewPort::ViewPort::PERSPECTIVE)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0, 0.5, 0.5, 0.5), ViewPort::ViewPort::TOP)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0, 0, 0.5, 0.5), ViewPort::ViewPort::FRONT)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0.5, 0, 0.5, 0.5), ViewPort::ViewPort::LEFT)
                    );

        _viewPorts.setViewPorts(viewPorts, surfaceSize);
    }


    void ModelScene::addMaterial(const MaterialInfo::Emissive & emissive,
                                    const MaterialInfo::Diffuse & diffuse,
                                    const MaterialInfo::Specular & specular,
                                    const MaterialInfo::Shininess & shininess) {
        _materials.push_back(new MaterialInfo::Material(emissive, diffuse, specular, shininess));
    }

    void ModelScene::addLightSource(const LightInfo::Position & position,
                                       const LightInfo::Color & color,
                                       const LightInfo::AmbientIntensity & ambientIntensity) {
        _lightSources.push_back(new LightInfo::LightSource(position, color, ambientIntensity));
    }

    void ModelScene::addTexture(TextureInfo::Texture & textureInfo) {
        textureInfo.pixelFormat = QOpenGLTexture::Red;

        QOpenGLTexture * texture = new QOpenGLTexture(textureInfo.target);

        _viewPorts.scale(textureInfo.scaling);

        texture->create();
        texture->setFormat(textureInfo.textureFormat);
        texture->setSize(textureInfo.size.x(), textureInfo.size.y(), textureInfo.size.z());

        texture->allocateStorage();

        texture->setMinMagFilters(QOpenGLTexture::LinearMipMapNearest, QOpenGLTexture::Linear);
        texture->setWrapMode(QOpenGLTexture::ClampToBorder);

        texture->setData(textureInfo.pixelFormat, textureInfo.pixelType,
                         (void *) textureInfo.mergedData.data(), &(textureInfo.pixelTransferOptions));

        texture->generateMipMaps();

        _textures.push_back(texture);
        _texturesInfo.insert(texture, textureInfo);
        _texturesInModel.insert(_selectedModel, texture);

        textureInfo.mergedData.clear();
    }

    void ModelScene::addStlModel(ModelInfo::BuffersVN buffers) {
        Model::StlModel * model = new Model::StlModel;

        // select newly created model
        _selectedModel = model;

        model->initModel<ModelInfo::BuffersVN>(buffers);

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

        _models.push_back(model);
    }

    void ModelScene::addHeadModel(SliceInfo::Slices slices) {
        Model::HeadModel * model = new Model::HeadModel;

        _selectedModel = model;

        // depth
        model->init(slices.texture.size.z());

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

        _models.push_back(model);
    }
}
