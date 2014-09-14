#ifndef EVALUATORMODEL_H
#define EVALUATORMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class EvaluatorModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit EvaluatorModel(Scene::AbstractScene * scene, AbstractModel * parent = nullptr,

                                const ShaderInfo::ShaderFiles & shaderFiles =
                                ShaderInfo::ShaderFiles(
                                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/evaluatorVertex.glsl"),
                                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/evaluatorFragment.glsl")
                                ),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames() << "vertex",

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames() << "vp" << "color");

        virtual void setSize(const QSize & size);
        virtual void setSize(const int & width, const int & height);

        virtual void setColor(const QVector3D & color);

        virtual void setStep(const qreal & stepX, const qreal & stepY = 0.0f);

        virtual void init() final;

    protected:
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;

        virtual void drawingRoutine() const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;

    private:
        int _width;
        int _height;

        QVector3D _color;

        qreal _stepX;
        qreal _stepY;
    };
}

#endif // EVALUATORMODEL_H
