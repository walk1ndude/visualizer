#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVN.h"

namespace Model {
    class StlModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit StlModel(PointsModel * points = nullptr,
                          AbstractModel * parent = nullptr,

                          const ShaderInfo::ShaderFiles & shaderFiles =
                          ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/stlVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/stlFragment.glsl")
                              << ShaderInfo::FragmentShaderFile(":shaders/helpersFragment.glsl")
                          ),

                          const ShaderInfo::ShaderVariablesNames & attributeArrays =
                          ShaderInfo::ShaderVariablesNames() << "vertex" << "normal",

                          const ShaderInfo::ShaderVariablesNames & uniformValues =
                          ShaderInfo::ShaderVariablesNames() << "colorU" << "mvp" << "normalMatrix");

        void init(ModelInfo::BuffersVN buffers);

    protected:
        void bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);
        void bindAttributeArrays(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();
    };
}
#endif // STLMODEL_H
