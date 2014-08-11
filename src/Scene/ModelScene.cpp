#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/HeadModel.h"
#include "Model/PointsModel.h"

#include <QtCore/QDateTime>

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
        if (_selectedModel) {
            _selectedModel->rotate(rotation);
        }
    }

    void ModelScene::setZoomFactor(const qreal & zoomFactor) {
        _viewPorts.zoom(zoomFactor);
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
        _viewPorts.resize(surfaceSize);

        ViewPort::ViewPort viewPort;
        ViewPort::ViewPortRect boundingRect;

        ViewPort::ViewPortsIterator itV (_viewPorts);
        QListIterator<Model::AbstractModel *> itM (_models);

        qint64 startTime = QDateTime::currentMSecsSinceEpoch();

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

        /* some children, like pointsmodel can change its values after rendering -
        * for example depth buffer check affects values of points (z-coordinate)
        */
        bool needToRedraw = false;

        while (itM.hasNext()) {
            itV.toFront();

            Model::AbstractModel * model = itM.next();

            while (itV.hasNext()) {
                needToRedraw |= model->checkDepthBuffer(itV.next());
            }
        }

        qDebug() << "rendering scene, time overall: " << QDateTime::currentMSecsSinceEpoch() - startTime << " ms";

        if (needToRedraw) {
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
