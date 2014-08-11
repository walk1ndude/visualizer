#ifndef ABSTRACTMODELWITHPOINTS_H
#define ABSTRACTMODELWITHPOINTS_H

#include "Model/AbstractModel.h"
#include "Model/PointsModel.h"

#include "Info/PointsInfo.h"

namespace ModelInfo {
    using ViewAxisRange = QVector2D;

    enum ViewAxis {
        XAXIS,
        YAXIS,
        ZAXIS
    };

    class ViewRange {
    public:
        ViewAxisRange xRange;
        ViewAxisRange yRange;
        ViewAxisRange zRange;

        ViewRange(const ViewAxisRange & xRange,
                  const ViewAxisRange & yRange,
                  const ViewAxisRange & zRange,
                  QOpenGLShaderProgram * program,
                  const ShaderInfo::ShaderVariablesNames & shaderVariables) :
            xRange(xRange), yRange(yRange), zRange(zRange),
            shaderXRange(program->uniformLocation(shaderVariables.at(0))),
            shaderYRange(program->uniformLocation(shaderVariables.at(1))),
            shaderZRange(program->uniformLocation(shaderVariables.at(2))) {
        }

        void setUniformValue(QOpenGLShaderProgram * program) {
            program->setUniformValue(shaderXRange, xRange);
            program->setUniformValue(shaderYRange, yRange);
            program->setUniformValue(shaderZRange, zRange);
        }

        void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                               const ModelInfo::ViewAxis viewAxis) {
            switch (viewAxis) {
            case ModelInfo::XAXIS:
                xRange = viewAxisRange;
                break;
            case ModelInfo::YAXIS:
                yRange = viewAxisRange;
                break;
            case ModelInfo::ZAXIS:
                zRange = viewAxisRange;
                break;
            }
        }

    private:
        ShaderInfo::ShaderVariable shaderXRange;
        ShaderInfo::ShaderVariable shaderYRange;
        ShaderInfo::ShaderVariable shaderZRange;
    };
}

namespace Model {
    class AbstractModelWithPoints : public AbstractModel {
        Q_OBJECT
    public:
        virtual void addPoint(const QString & name, PointsInfo::ModelPoint * point);

        virtual ModelInfo::ViewAxisRange correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange);

        virtual void setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                  const ModelInfo::ViewAxisRange & yRange,
                                  const ModelInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariablesNames & shaderVariables) final;

        virtual void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                      const ModelInfo::ViewAxis viewAxis = ModelInfo::XAXIS) final;

        virtual void processChildren() final;

    protected:
        AbstractModelWithPoints(PointsModel * points = nullptr,
                                AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles());

        virtual void initShaderVariables(QOpenGLShaderProgram * program) = 0;
        virtual void setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) = 0;
        virtual void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) = 0;

        virtual void setShaderVariables();

        virtual PointsModel * pointsModel() final;
        virtual PointsInfo::ModelPoints modelPoints() final;
        
        virtual bool checkDepthBuffer(ViewPort::ViewPort & viewPort) final;

    private:
        PointsModel * _points;

        PointsInfo::ModelPoints _modelPoints;

        ModelInfo::ViewRange * _viewRange;
        
    signals:
        void pointUpdated(const PointsInfo::UpdatedPoint & point);
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
