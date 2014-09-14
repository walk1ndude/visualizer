#ifndef EVALUATORMODEL_H
#define EVALUATORMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class EvaluatorModel : public AbstractModel {
        Q_OBJECT

    public:
        explicit EvaluatorModel(Scene::AbstractScene * scene, AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(

                                ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/evaluatorVertex.glsl"),
                                ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/evaluatorFragment.glsl")),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames() << "vertex",

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames() << "vp" << "color");

        void setSize(const QSize & size);
        void setSize(const int & width, const int & height);

        void setColor(const QVector3D & color);

        void setStep(const qreal & stepX, const qreal & stepY = 0.0f);

        void init();

    protected:
        void bindAttributeArrays(QOpenGLShaderProgram * program);
        void bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);

        virtual void drawingRoutine();

        void glStatesEnable();
        void glStatesDisable();

    private:
        int _width;
        int _height;

        QVector3D _color;

        qreal _stepX;
        qreal _stepY;
    };
}

#endif // EVALUATORMODEL_H
