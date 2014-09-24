#include "Model/AbstractModelWithPoints.h"

#include <cmath>

ShaderInfo::ShaderVariablesNames appendToNames(const ShaderInfo::ShaderVariablesNames & names) {
    ShaderInfo::ShaderVariablesNames appended = names;
    appended << ShaderInfo::ShaderVariableName("points") << ShaderInfo::ShaderVariableName("pointsCount");
    return appended;
}

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(Scene::AbstractScene * scene,
                                                     PointsModel * points, AbstractModel * parent,
                                                     const ShaderInfo::ShaderFiles & shaderFiles,
                                                     const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                                     const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(scene, parent, shaderFiles, shaderAttributeArrays, appendToNames(shaderUniformValues)),
        _points(points),
        _pointsTexture(nullptr) {

    }

    PointsModel * AbstractModelWithPoints::pointsModel() {
        return _points;
    }
    
    PointsInfo::ModelPoints * AbstractModelWithPoints::modelPoints() {
        return &_modelPoints;
    }

    void AbstractModelWithPoints::addPoint(const PointsInfo::Name & name, PointsInfo::ModelPoint * point) {
        _modelPoints.insert(name, point);
    }

    void AbstractModelWithPoints::hidePoint(const PointsInfo::Name & point) {
        _modelPoints.togglePoint(point);

        queueForUpdate();
    }
    
    void AbstractModelWithPoints::updateRoutine() {
        _points->init(&_modelPoints);

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

                int stencil = posZ & 0x0000000F;

                if (stencil != id() + 1) {
                    return false;
                }

                int depth = posZ >> 8;

                modelPoint->position.setZ(depth / (0xFFFFFF * 1.0f));

                if (Camera::Camera::unproject(modelPoint->position, projection(viewport) * lightView(viewport) * model(viewport), unprojectedPoint)) {
                    modelPoint->positionCalculated(unprojectedPoint);

                    updateNeeded = true;

                    emit pointUpdated(PointsInfo::UpdatedPoint(modelPoint->position * scene()->scalingFactor(), modelPoints()->key(modelPoint), id()));
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
            _viewRange = new ViewRangeInfo::ViewRange(xRange, yRange, zRange,
                                                  program(), shaderVariables);
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
}
