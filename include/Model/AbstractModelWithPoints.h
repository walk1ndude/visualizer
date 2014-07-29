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
    private:
        ShaderInfo::ShaderVariable shaderXRange;
        ShaderInfo::ShaderVariable shaderYRange;
        ShaderInfo::ShaderVariable shaderZRange;
    public:
        ViewAxisRange xRange;
        ViewAxisRange yRange;
        ViewAxisRange zRange;

        ViewRange(const ViewAxisRange & xRange,
                  const ViewAxisRange & yRange,
                  const ViewAxisRange & zRange,
                  QOpenGLShaderProgram * program,
                  const ShaderInfo::ShaderVariablesNames & shaderVariables) {
            this->xRange = xRange;
            this->yRange = yRange;
            this->zRange = zRange;

            shaderXRange = program->uniformLocation(shaderVariables.at(0));
            shaderYRange = program->uniformLocation(shaderVariables.at(1));
            shaderZRange = program->uniformLocation(shaderVariables.at(2));
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
    };
}

namespace Model {
    class AbstractModelWithPoints : public AbstractModel {
        Q_OBJECT
    public:
        virtual void addPoint(const QString & name, PointsInfo::ModelPoint * point);

        virtual void initShaderVariables(QOpenGLShaderProgram * program) = 0;
        virtual void setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) = 0;
        virtual void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) = 0;

        virtual ModelInfo::ViewAxisRange correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange);

        virtual void setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                  const ModelInfo::ViewAxisRange & yRange,
                                  const ModelInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariablesNames & shaderVariables) final;

        virtual void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                      const ModelInfo::ViewAxis viewAxis = ModelInfo::XAXIS) final;

    protected:
        AbstractModelWithPoints(PointsModel * points = nullptr,
                                AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles());

        virtual void setShaderVariables();

        virtual PointsModel * pointsModel() final;
        virtual PointsInfo::ModelPoints modelPoints() final;
        
        virtual void setChildrenVariables() final;
        
        virtual bool checkDepthBuffer(ViewPort::ViewPort & viewPort) final;

    private:
        PointsModel * _points;

        PointsInfo::ModelPoints _modelPoints;

        ModelInfo::ViewRange * _viewRange;
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
