#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVN.h"

namespace Model {
    class StlModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit StlModel(Scene::AbstractScene * scene,
                          AbstractModel * parent = nullptr,

                          const ShaderInfo::ShaderFiles & shaderFiles =
                          ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Stl/vertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Stl/fragment.glsl")
                              << ShaderInfo::FragmentShaderFile(":shaders/Helpers/fragment.glsl")
                          ),

                          const ShaderInfo::ShaderVariablesNames & attributeArrays =
                          ShaderInfo::ShaderVariablesNames() << "vertex" << "normal",

                          const ShaderInfo::ShaderVariablesNames & uniformValues =
                          ShaderInfo::ShaderVariablesNames() << "colorU" << "model" << "view" << "projection" << "normalMatrix" << "eye" << "lightView");

        //void init(ModelInfo::BuffersVN buffers);

        void init(const ModelInfo::Params & params);
    
    protected:
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
    };
}
#endif // STLMODEL_H
