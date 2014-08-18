#include "Model/AbstractModelWithPoints.h"

#include <cmath>

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(PointsModel * points, AbstractModel * parent,
                                                     const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {
        _points = points;
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
    }
    
    bool AbstractModelWithPoints::checkDepthBuffer(Viewport::Viewport * viewport) {
        QVector4D unprojectedPoint;
        
        bool updateNeeded = false;
       
        for (PointsInfo::ModelPoint * modelPoint : modelPoints()) {
            if (modelPoint->viewport == viewport && !modelPoint->isPositionCalculated()) {
                GLushort posZ;

                Viewport::ViewportRect boundingRect = viewport->boundingRect();

                // usage of GL_UNSIGNED_SHORT explaned here http://www.opengl.org/wiki/Common_Mistakes#Depth_Buffer_Precision
                glReadPixels(
                             std::round(boundingRect.x() + modelPoint->position.x() * boundingRect.width()),
                             std::round(boundingRect.y() + modelPoint->position.y() * boundingRect.height()),
                             1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &posZ
                             );
                
                qDebug() << modelPoint->position << posZ;

                modelPoint->position.setZ(posZ / 65536.0f);

                modelPoint->position = viewport->placeXYZAccordingToViewport(modelPoint->position);

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

}
