#include "Scene/ModelScene.h"

#include "Model/StlModel.h"

namespace Scene {
    ModelScene::ModelScene() :
        AbstractScene(),
        _zoomFactor(2.0) {

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
            if (viewPort.projectionType() == ViewPort::ViewPort::LEFT) {
                //_screenSaveRect = boundingRect;
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
        if (_textures.keys().size()) {
            qDeleteAll(_textures.keys().begin(), _textures.keys().end());
            _textures.clear();
        }

        qDeleteAll(_models.begin(), _models.end());
        _models.clear();

        qDeleteAll(_materials.begin(), _materials.end());
        _materials.clear();

        qDeleteAll(_lightSources.begin(), _lightSources.end());
        _lightSources.clear();
    }

    // need some good defaults here
    void ModelScene::initMaterials() {
        addMaterial(MaterialInfo::Emissive(0.8, 0.8, 0.8, 0.8),
                    MaterialInfo::Diffuse(1.0, 1.0, 1.0, 0.8),
                    MaterialInfo::Specular(0.7, 0.7, 0.7, 0.7),
                    MaterialInfo::Shininess(50.0));
    }

    void ModelScene::initLightSources() {
        addLightSource(LightInfo::Position(10.0, 10.0, -10.0, 1.0),
                       LightInfo::Position(1.0, 1.0, 1.0, 1.0),
                       LightInfo::AmbientIntensity((GLfloat) 4.3));
    }

    void ModelScene::initializeViewPorts(const QSize & surfaceSize) {
        ViewPort::ViewPorts viewPorts;

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0.5, 0.5, 0.5, 0.5), ViewPort::ViewPort::PERSPECTIVE)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ViewPort::ProjectionType>(
                        QRectF(0, 0.5, 0.5, 0.5), ViewPort::ViewPort::BOTTOM)
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
        QOpenGLTexture * texture = new QOpenGLTexture(textureInfo.target);

        _textures.insert(texture, textureInfo);
        _texturesInModel.insert(_selectedModel, texture);
    }

    void ModelScene::addStlModel(ModelInfo::BuffersVN buffers) {
        Model::StlModel * model = new Model::StlModel;

        model->createModel<ModelInfo::BuffersVN>(buffers);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariables() << "lightSource.location" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariables() << "stlMaterial.emissive" << "stlMaterial.diffuse" <<
                           "stlMaterial.specular" << "stlMaterial.shininess");

        model->setViewRange(ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariables() << "xRange" << "yRange" << "zRange");

        _models.push_back(model);
    }
}
