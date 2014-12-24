#ifndef EVALUATORMODEL_H
#define EVALUATORMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class EvaluatorModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit EvaluatorModel(Scene::AbstractScene * scene,

                                const ShaderInfo::ShaderFiles & shaderFiles =
                                ShaderInfo::ShaderFiles(
                                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Evaluator/vertex.glsl"),
                                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Evaluator/fragment.glsl")
                                ),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames() << "vertex",

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames() << "mvp" << "color");

        virtual void setSize(const QSize & size);
        virtual void setSize(const int & width, const int & height);

        virtual void setColor(const QColor & color);

        virtual void setStep(const qreal & stepX, const qreal & stepY = 0.0f);

        virtual void init(const ModelInfo::Params & params);

    protected:
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;

        virtual void drawingRoutine() const;

        virtual Camera::ModelMatrix model(const Viewport::Viewport * viewport) const;

    private:
        int _width;
        int _height;

        QColor _color;

        qreal _stepX;
        qreal _stepY;
    };
}

#endif // EVALUATORMODEL_H
