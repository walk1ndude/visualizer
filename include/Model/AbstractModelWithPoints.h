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
    public:
        virtual void addPoint(const QString & name, PointsInfo::ModelPoint * point);

        virtual void setViewRange(const ViewRangeInfo::ViewAxisRange & xRange,
                                  const ViewRangeInfo::ViewAxisRange & yRange,
                                  const ViewRangeInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariablesNames & shaderVariables);

        virtual void setViewAxisRange(const ViewRangeInfo::ViewAxisRange & viewAxisRange,
                                      const ViewRangeInfo::ViewAxis viewAxis = ViewRangeInfo::XAXIS);

        virtual void processChildren() final;

    protected:
        AbstractModelWithPoints(PointsModel * points = nullptr,
                                AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles());

        virtual void initShaderVariables(QOpenGLShaderProgram * program);
        virtual void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewPort);
        virtual void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) = 0;

        virtual void setShaderVariables();

        virtual PointsModel * pointsModel() final;
        virtual PointsInfo::ModelPoints modelPoints() final;
        
        virtual bool checkDepthBuffer(Viewport::Viewport * viewport) final;

        virtual void deleteModel();

    private:
        ShaderInfo::ShaderVariable _shaderPointsCount;
        ShaderInfo::ShaderVariable _shaderPoints;

        PointsModel * _points;

        PointsInfo::ModelPoints _modelPoints;

        ViewRangeInfo::ViewRange * _viewRange;

        QOpenGLTexture * _pointsTexture;

        void updatePointsTexture();
        
    signals:
        void pointUpdated(const PointsInfo::UpdatedPoint & point);

    public slots:
        virtual void update();
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
