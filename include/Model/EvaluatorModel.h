#ifndef EVALUATORMODEL_H
#define EVALUATORMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class EvaluatorModel : public AbstractModel {
        Q_OBJECT

    public:
        explicit EvaluatorModel(AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(

                                ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/evaluatorVertex.glsl"),
                                ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/evaluatorFragment.glsl")),

                                const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames() << "vertex",

                                const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames() << "mvp" << "color");

        void setSize(const QSize & size);
        void setSize(const int & width, const int & height);

        void setColor(const QVector4D & color);

    protected:
        virtual void drawingRoutine();
    };
}

#endif // EVALUATORMODEL_H
