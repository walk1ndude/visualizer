#include "Scene/ModelScene.h"

#include "Model/StlModel.h"
#include "Model/HeadModel.h"
#include "Model/PointsModel.h"
#include "Model/EvaluatorModel.h"

namespace Scene {
    ModelScene::ModelScene() :
        AbstractScene() {
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

    void ModelScene::updateScene() {
        for (Model::AbstractModel * model : _models) {
            if (model->updateNeeded()) {
                model->update();
            }
        }
    }

    void ModelScene::renderScene(const QSize & surfaceSize) {
        viewportArray()->resize(surfaceSize);

        updateScene();

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

        _materials.clear();
        _lightSources.clear();
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

    void ModelScene::addStlModel(ModelInfo::BuffersVN buffers) {
        Model::PointsModel * pointsInModel = new Model::PointsModel;
        _models.append(pointsInModel);

        Model::StlModel * model = new Model::StlModel(pointsInModel);

        selectModel(model);

        model->init(buffers);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariablesNames() << "lightSource.position" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariablesNames() << "material.emissive" << "material.diffuse" <<
                           "material.specular" << "material.shininess");

        model->setViewRange(ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
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
                           ShaderInfo::ShaderVariablesNames() << "material.emissive" << "material.diffuse" <<
                           "material.specular" << "material.shininess");

        model->addTexture(_textures.at(0),
                          ShaderInfo::ShaderVariablesNames() << "texHead");

        model->setViewRange(ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ViewRangeInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariablesNames() << "ranges.xRange" << "ranges.yRange" << "ranges.zRange");
        
        
        QObject::connect(model, &Model::HeadModel::pointUpdated, this, &Scene::AbstractScene::pointUpdated, Qt::DirectConnection);

        _models.append(model);
    }

    void ModelScene::addEvaluatorModel(const int & width, const int & height,
                                       const qreal & stepX, const qreal & stepY,
                                       const QVector3D & color) {
        Model::EvaluatorModel * model = new Model::EvaluatorModel;

        model->setSize(width, height);
        model->setStep(stepX, stepY);
        model->setColor(color);

        model->init();

        _models.append(model);
    }

    void ModelScene::initScene() {
        addEvaluatorModel();
    }
}
