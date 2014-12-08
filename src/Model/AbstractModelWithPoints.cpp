#include "Model/AbstractModelWithPoints.h"

#include <cmath>

ShaderInfo::ShaderVariablesNames appendToNames(const ShaderInfo::ShaderVariablesNames & names) {
    ShaderInfo::ShaderVariablesNames appended = names;
    appended << ShaderInfo::ShaderVariableName("points") << ShaderInfo::ShaderVariableName("pointsCount");
    return appended;
}

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(Scene::AbstractScene * scene,
                                                     const ShaderInfo::ShaderFiles & shaderFiles,
                                                     const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                                     const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(scene, shaderFiles, shaderAttributeArrays, appendToNames(shaderUniformValues)),
        _pointsTexture(nullptr) {

    }

    void AbstractModelWithPoints::init(const ModelInfo::Params & params) {
        AbstractModel::init(params);

        QVariantMap rangesMap = params["viewRanges"].toMap();

        QVariantMap viewMap = params["viewRangeShader"].toMap();

        setViewRange(
                    (rangesMap.contains("x") ? rangesMap["x"].value<ViewRangeInfo::ViewAxisRange>() : DEFAULT_VIEWAXISRANGE),
                    (rangesMap.contains("y") ? rangesMap["y"].value<ViewRangeInfo::ViewAxisRange>() : DEFAULT_VIEWAXISRANGE),
                    (rangesMap.contains("z") ? rangesMap["z"].value<ViewRangeInfo::ViewAxisRange>() : DEFAULT_VIEWAXISRANGE),
                    ShaderInfo::ShaderVariablesNames()
                        << viewMap["x"].value<ShaderInfo::ShaderVariableName>()
                        << viewMap["y"].value<ShaderInfo::ShaderVariableName>()
                        << viewMap["z"].value<ShaderInfo::ShaderVariableName>());
    }

    PointsModel * AbstractModelWithPoints::pointsModel() const {
        return _points;
    }

    void AbstractModelWithPoints::addChild(AbstractModel * child) {
        if (PointsModel * pointsModel = qobject_cast<PointsModel *>(child)) {
            _points = pointsModel;
        }

        AbstractModel::addChild(child);
    }

    void AbstractModelWithPoints::addPoint(const PointsInfo::PointID & id, PointsInfo::ModelPoint * point) {
        _modelPoints.insert(id, point);
    }

    void AbstractModelWithPoints::setPoint(const PointsInfo::PointID & id, const PointsInfo::Position3D & position, Viewport::Viewport * viewport) {
        PointsInfo::ModelPoint * point = _modelPoints[id];

        point->position = position;
        point->viewport = viewport;

        point->queueToRecalculate();

        queueForUpdate();
    }

    void AbstractModelWithPoints::togglePoint(const PointsInfo::PointID & point) {
        _modelPoints.togglePoint(point);

        queueForUpdate();
    }
    
    void AbstractModelWithPoints::updateRoutine() {
        _points->init(ModelInfo::Params() = {
            { "modelPoints", QVariant::fromValue(&_modelPoints) }
        });

        updatePointsTexture(program());
    }

    void AbstractModelWithPoints::updatePointsTexture(QOpenGLShaderProgram * program) {
        int pointsCount = _modelPoints.size();
        
        if (!pointsCount) {
            return;
        }

        if (!_pointsTexture) {
            _pointsTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        }
    
        if (_pointsTexture->isStorageAllocated()) {
            _pointsTexture->destroy();
        }
        
        _pointsTexture->create();
        _pointsTexture->setFormat(QOpenGLTexture::RGBA32F);

        _pointsTexture->setSize(2, pointsCount);
        _pointsTexture->allocateStorage();
        
        float * data = new float[pointsCount * 8];

        int i = 0;

        for (const PointsInfo::ModelPoint * modelPoint : _modelPoints.points()) {
            data[i ++] = modelPoint->position.x();
            data[i ++] = modelPoint->position.y();
            data[i ++] = modelPoint->position.z();
            
            data[i ++] = modelPoint->radius;

            data[i ++] = modelPoint->color.redF();
            data[i ++] = modelPoint->color.greenF();
            data[i ++] = modelPoint->color.blueF();
            data[i ++] = modelPoint->color.alphaF();
        }
        
        _pointsTexture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::Float32, (void *) data);
        _pointsTexture->setWrapMode(QOpenGLTexture::Repeat);
        _pointsTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
       
        _pointsTexture->bind(_pointsTexture->textureId());

        _pointsTexture->generateMipMaps();

        delete [] data;

        /* for obvious reasons we can't do it anywhere else - or we may
        get different values for pointsCount during (un) hide point operations */
        program->setUniformValue(uniformValues["pointsCount"], pointsCount);
    }

    bool AbstractModelWithPoints::checkDepthBuffer(const Viewport::Viewport * viewport) {
        PointsInfo::Position3D unprojectedPoint;

        bool updateNeeded = false;

        for (PointsInfo::ModelPoint * modelPoint : _modelPoints.points()) {
            if (modelPoint->viewport == viewport && !modelPoint->isPositionCalculated()) {
                GLint posZ;

                Viewport::ViewportRect boundingRect = viewport->boundingRect();

                PointsInfo::Position2D rounded = PointsInfo::Position2D(
                            std::round(boundingRect.x() + modelPoint->position.x() * boundingRect.width()),
                            std::round(boundingRect.y() + modelPoint->position.y() * boundingRect.height())
                            );

                glReadPixels(rounded.x(), rounded.y(), 1, 1, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, &posZ);

                uint stencil = posZ & 0x0000000F;

                if (stencil != numberedID() + 1) {
                    return false;
                }

                uint depth = posZ >> 8;

                modelPoint->position.setZ(depth / (0xFFFFFF * 1.0f));

                if (Camera::Camera::unproject(modelPoint->position, mvp(viewport), unprojectedPoint)) {
                    modelPoint->positionCalculated(unprojectedPoint);

                    updateNeeded = true;

                    Message::SettingsMessage message(
                                Message::Sender(id()),
                                Message::Reciever("settingsJS"),
                                Message::Recievers() = { "measures" }
                    );

                    message.data["action"] = "updatePoint";
                    message.data["params"] = ModelInfo::Params() = {
                        { "id", _modelPoints.key(modelPoint) },
                        { "position", modelPoint->position * scene()->scalingFactor() }
                    };

                    emit post(message);
                }
            }
        }

        if (updateNeeded) {
            queueForUpdate();
        }

        return updateNeeded;
    }

    void AbstractModelWithPoints::setViewAxisRange(const ViewRangeInfo::ViewAxisRange & viewAxisRange,
                                                   const ViewRangeInfo::ViewAxis viewAxis) {
        _viewRange->setViewAxisRange(viewAxisRange, viewAxis);
    }

    void AbstractModelWithPoints::setViewRange(const ViewRangeInfo::ViewAxisRange & xRange,
                                               const ViewRangeInfo::ViewAxisRange & yRange,
                                               const ViewRangeInfo::ViewAxisRange & zRange,
                                               const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        QMutexLocker locker(&modelMutex);

        if (program()) {
            _viewRange = new ViewRangeInfo::ViewRange(xRange, yRange, zRange, program(), shaderVariables);
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
    }

    void AbstractModelWithPoints::bindUniformValues() const {
        _viewRange->setUniformValue(program());

        AbstractModel::bindUniformValues();
    }

    void AbstractModelWithPoints::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * ) const {
        if (_pointsTexture) {
            program->setUniformValue(uniformValues["points"], _pointsTexture->textureId());
        }
    }

    void AbstractModelWithPoints::deleteModel() {
        _pointsTexture->release(_pointsTexture->textureId());
        _pointsTexture->destroy();

        AbstractModel::deleteModel();
    }

    void AbstractModelWithPoints::glStatesEnable() const {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        AbstractModel::glStatesEnable();
    }

    void AbstractModelWithPoints::glStatesDisable() const {
        glDisable(GL_DEPTH_TEST);

        AbstractModel::glStatesDisable();
    }

    void AbstractModelWithPoints::invoke(const QString & name, const ModelInfo::Params & params) {
        if (name == "setRange") {
            ViewRangeInfo::ViewAxis viewAxis;

            QString axis = params["axis"].toString();

            if (axis.startsWith("y")) {
                viewAxis = ViewRangeInfo::ViewAxis::YAXIS;
            }
            else if (axis.startsWith("z")) {
                viewAxis = ViewRangeInfo::ViewAxis::ZAXIS;
            }
            else {
                viewAxis = ViewRangeInfo::ViewAxis::XAXIS;
            }

            setViewAxisRange(params["range"].value<ViewRangeInfo::ViewAxisRange>(), viewAxis);
            return;
        }

        if (name == "addPoint") {
            addPoint(params["id"].value<PointsInfo::PointID>(),
                     new PointsInfo::ModelPoint(
                         params["color"].value<QColor>(),
                         params["groups"].value<PointsInfo::Groups>()
                         )
                     );
            return;
        }

        if (name == "setPoint") {
            setPoint(params["id"].value<PointsInfo::PointID>(),
                     params["position"].value<PointsInfo::Position3D>(),
                     params["viewport"].value<Viewport::Viewport *>()
            );

            return;
        }

        if (name == "togglePoint") {
            togglePoint(params["name"].value<PointsInfo::PointID>());

            return;
        }

        AbstractModel::invoke(name, params);
    }
}
