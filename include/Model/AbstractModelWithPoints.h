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
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames(),

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames());

        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewPort) const;
        virtual void bindUniformValues() const;

        virtual PointsModel * pointsModel() const final;

        virtual Camera::Matrix mvp(const Viewport::Viewport * viewport) const;
        
        virtual bool checkBuffers(const Viewport::Viewport * viewport) final;

        virtual void deleteModel();

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;

        virtual void init(const ModelInfo::Params & params);

    private:
        PointsModel * _points;

        PointsInfo::ModelPoints _modelPoints;

        ViewRangeInfo::ViewRange * _viewRange;

        QOpenGLTexture * _pointsTexture;

        virtual void updatePointsTexture(QOpenGLShaderProgram * program) final;

    public slots:
        virtual void addPoint(const PointsInfo::PointID & id, PointsInfo::ModelPoint * point) final;
        virtual void setPoint(const PointsInfo::PointID & id, const PointsInfo::Position3D & position, Viewport::Viewport * viewport) final;
        virtual void togglePoint(const PointsInfo::PointID & id) final;

        virtual void addChild(AbstractModel * child);

        virtual void setViewRange(const ViewRangeInfo::ViewAxisRange & xRange,
                                  const ViewRangeInfo::ViewAxisRange & yRange,
                                  const ViewRangeInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariablesNames & shaderVariables);

        virtual void setViewAxisRange(const ViewRangeInfo::ViewAxisRange & viewAxisRange,
                                      const ViewRangeInfo::ViewAxis viewAxis = ViewRangeInfo::XAXIS);

        virtual void updateRoutine() final;

        virtual void invoke(const QString & name, const ModelInfo::Params & params = ModelInfo::Params());
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
