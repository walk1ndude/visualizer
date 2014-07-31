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
    
    bool AbstractModelWithPoints::checkDepthBuffer(ViewPort::ViewPort & viewPort) {
        QVector4D unprojectdPoint;
        
        bool updateNeeded = false;
       
        foreach (PointsInfo::ModelPoint * modelPoint, modelPoints()) {
            if (viewPort.pointInViewPort(modelPoint->position) && !modelPoint->isPositionCalculated()) {
                GLushort posZ;
                
                modelPoint->position.setX(std::round(modelPoint->position.x()));
                modelPoint->position.setY(std::round(modelPoint->position.y()));
                
                // usage of GL_UNSIGNED_SHORT explaned here http://www.opengl.org/wiki/Common_Mistakes#Depth_Buffer_Precision
                glReadPixels(
                             (int) modelPoint->position.x(),
                             (int) modelPoint->position.y(),
                             1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &posZ
                             );
                
                modelPoint->position.setZ(posZ / 65536.0f);

                if (viewPort.unproject(modelPoint->position, unprojectdPoint)) {
                    modelPoint->position = QVector3D(unprojectdPoint);
                    modelPoint->positionCalculated();
                    
                    updateNeeded = true;
                    
                    emit pointCalculated(PointsInfo::CalcalutedPoint(modelPoint->position, modelPoints().key(modelPoint), id()));
                }
            }
        }

        if (updateNeeded) {
            _points->queueForUpdate();
        }

        processChildren();
        
        return updateNeeded;
    }

    ModelInfo::ViewAxisRange AbstractModelWithPoints::correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange) {
        return viewAxisRange;
    }

    void AbstractModelWithPoints::setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                                       const ModelInfo::ViewAxis viewAxis) {
        _viewRange->setViewAxisRange(correctedViewwAxisRange(viewAxisRange), viewAxis);
    }

    void AbstractModelWithPoints::setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                               const ModelInfo::ViewAxisRange & yRange,
                                               const ModelInfo::ViewAxisRange & zRange,
                                               const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (program()) {
            _viewRange = new ModelInfo::ViewRange(correctedViewwAxisRange(xRange),
                                                  correctedViewwAxisRange(yRange),
                                                  correctedViewwAxisRange(zRange),
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
