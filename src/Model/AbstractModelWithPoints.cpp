#include "Model/AbstractModelWithPoints.h"

#include <cmath>

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(PointsModel * points, AbstractModel * parent,
                                                     const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {
        _points = points;
        _pointsTexture = nullptr;
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
    
        if (_pointsTexture->isStorageAllocated()) {
            _pointsTexture->destroy();
        }
        
        _pointsTexture->create();
        _pointsTexture->setFormat(QOpenGLTexture::RGBA32F);
        // columns: point coords, radius, color
        _pointsTexture->setSize(2, pointsCount);
        _pointsTexture->allocateStorage();
        
        float * data = new float[8 * pointsCount];

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
        _pointsTexture->bind(_pointsTexture->textureId());

        _pointsTexture->generateMipMaps();

        delete data;
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
                
                qDebug() << modelPoint->position << "before";

                if (viewport->unproject(modelPoint->position, unprojectedPoint)) {
                    modelPoint->positionCalculated(unprojectedPoint);
                    
                    updateNeeded = true;
                    
                    emit pointUpdated(PointsInfo::UpdatedPoint(modelPoint->position, modelPoints().key(modelPoint), id()));
                }
            }
        }

        if (updateNeeded) {
            _points->queueForUpdate();
        }

        processChildren();
        
        return updateNeeded;
    }

    void AbstractModelWithPoints::setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                                   const ModelInfo::ViewAxis viewAxis) {
        _viewRange->setViewAxisRange(viewAxisRange, viewAxis);
    }

    void AbstractModelWithPoints::setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                               const ModelInfo::ViewAxisRange & yRange,
                                               const ModelInfo::ViewAxisRange & zRange,
                                               const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (program()) {
            _viewRange = new ModelInfo::ViewRange(xRange, yRange, zRange,
                                                  program(), shaderVariables);
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
    }

    void AbstractModelWithPoints::setShaderVariables() {
        _viewRange->setUniformValue(program());

        AbstractModel::setShaderVariables();
    }

    void AbstractModelWithPoints::initShaderVariables(QOpenGLShaderProgram * program) {
        _pointsTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);

        _shaderPoints = program->uniformLocation("points");
        _shaderPointsCount = program->uniformLocation("pointsCount");
    }

    void AbstractModelWithPoints::setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * ) {
        program->setUniformValue(_shaderPoints, _pointsTexture->textureId());
        program->setUniformValue(_shaderPointsCount, _modelPoints.size());
    }

    void AbstractModelWithPoints::deleteModel() {
        _pointsTexture->release(_pointsTexture->textureId());
        _pointsTexture->destroy();

        AbstractModel::deleteModel();
    }

    void AbstractModelWithPoints::update() {

    }
}
