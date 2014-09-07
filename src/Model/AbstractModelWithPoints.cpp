#include "Model/AbstractModelWithPoints.h"

#include <cmath>

ShaderInfo::ShaderVariablesNames appendToNames(const ShaderInfo::ShaderVariablesNames & names) {
    ShaderInfo::ShaderVariablesNames appended = names;
    appended << ShaderInfo::ShaderVariableName("points") << ShaderInfo::ShaderVariableName("pointsCount");
    return appended;
}

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(PointsModel * points, AbstractModel * parent,
                                                     const ShaderInfo::ShaderFiles & shaderFiles,
                                                     const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                                     const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel::AbstractModel(parent, shaderFiles, shaderAttributeArrays, appendToNames(shaderUniformValues)),
        _points(points),
        _pointsTexture(nullptr) {

    }

    PointsModel * AbstractModelWithPoints::pointsModel() {
        return _points;
    }
    
    PointsInfo::ModelPoints AbstractModelWithPoints::modelPoints() {
        return _modelPoints;
    }

    void AbstractModelWithPoints::addPoint(const QString & name, PointsInfo::ModelPoint * point) {
        _modelPoints.insert(name, point);
    }
    
    void AbstractModelWithPoints::processChildren() {
        _points->fillBuffers(_modelPoints);

        updatePointsTexture();
    }

    void AbstractModelWithPoints::updatePointsTexture() {
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

        for (const PointsInfo::ModelPoint * modelPoint : _modelPoints) {
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
    }
    
    bool AbstractModelWithPoints::checkDepthBuffer(Viewport::Viewport * viewport) {
        QVector4D unprojectedPoint;

        bool updateNeeded = false;
       
        for (PointsInfo::ModelPoint * modelPoint : _modelPoints) {
            if (modelPoint->viewport == viewport && !modelPoint->isPositionCalculated()) {
                GLushort posZ;

                Viewport::ViewportRect boundingRect = viewport->boundingRect();

                // usage of GL_UNSIGNED_SHORT explaned here http://www.opengl.org/wiki/Common_Mistakes#Depth_Buffer_Precision
                glReadPixels(std::round(boundingRect.x() + modelPoint->position.x() * boundingRect.width()),
                             std::round(boundingRect.y() + modelPoint->position.y() * boundingRect.height()),
                             1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &posZ);

                modelPoint->position.setZ(posZ / 65535.0f);
                
                if (viewport->unproject(modelPoint->position, projection(viewport) * view(viewport) * model(viewport), unprojectedPoint)) {
                    modelPoint->positionCalculated(unprojectedPoint);
                    
                    updateNeeded = true;
                    
                    emit pointUpdated(PointsInfo::UpdatedPoint(modelPoint->position, modelPoints().key(modelPoint), id()));
                }
            }
        }

        if (updateNeeded) {
            _points->queueForUpdate();
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

    void AbstractModelWithPoints::bindUniformValues() {
        _viewRange->setUniformValue(program());

        AbstractModel::bindUniformValues();
    }

    void AbstractModelWithPoints::bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * ) {
        if (_pointsTexture) {
            program->setUniformValue(uniformValues["points"], _pointsTexture->textureId());
            program->setUniformValue(uniformValues["pointsCount"], _modelPoints.size());
        }
    }

    void AbstractModelWithPoints::deleteModel() {
        _pointsTexture->release(_pointsTexture->textureId());
        _pointsTexture->destroy();

        AbstractModel::deleteModel();
    }
}
