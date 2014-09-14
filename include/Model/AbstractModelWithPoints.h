#ifndef ABSTRACTMODELWITHPOINTS_H
#define ABSTRACTMODELWITHPOINTS_H

#include <QtGui/QOpenGLTexture>

#include "Model/AbstractModel.h"
#include "Model/PointsModel.h"

#include "Info/PointsInfo.h"
#include "Info/ViewRangeInfo.h"

namespace Model {
    class AbstractModelWithPoints : public AbstractModel {
        Q_OBJECT

    protected:
        explicit AbstractModelWithPoints(Scene::AbstractScene * scene,
                                PointsModel * points = nullptr,
                                AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames(),

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames());

        virtual void bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewPort);
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) = 0;

        virtual void bindUniformValues();

        virtual PointsModel * pointsModel() final;
        virtual PointsInfo::ModelPoints * modelPoints() final;
        
        virtual bool checkDepthBuffer(Viewport::Viewport * viewport) final;

        virtual void deleteModel();

    private:
        PointsModel * _points;

        PointsInfo::ModelPoints _modelPoints;

        ViewRangeInfo::ViewRange * _viewRange;

        QOpenGLTexture * _pointsTexture;

        void updatePointsTexture(QOpenGLShaderProgram * program);
        
    signals:
        void pointUpdated(const PointsInfo::UpdatedPoint & point);

    public slots:
        virtual void addPoint(const PointsInfo::Name & name, PointsInfo::ModelPoint * point) final;
        virtual void hidePoint(const PointsInfo::Name & point) final;

        virtual void setViewRange(const ViewRangeInfo::ViewAxisRange & xRange,
                                  const ViewRangeInfo::ViewAxisRange & yRange,
                                  const ViewRangeInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariablesNames & shaderVariables);

        virtual void setViewAxisRange(const ViewRangeInfo::ViewAxisRange & viewAxisRange,
                                      const ViewRangeInfo::ViewAxis viewAxis = ViewRangeInfo::XAXIS);

        virtual void updateRoutine() final;
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
