#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class PointsModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit PointsModel(AbstractModel * parent = nullptr,
                             const ShaderInfo::ShaderFiles & shaderFiles =
                             ShaderInfo::ShaderFiles(
                                ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/pointsVertex.glsl"),
                                ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/pointsFragment.glsl"),
                                ShaderInfo::GeometryShaderFiles() << ShaderInfo::GeometryShaderFile(":shaders/pointsGeometry.glsl")
                             ),

                             const ShaderInfo::ShaderVariablesNames & attributeArrays =
                                ShaderInfo::ShaderVariablesNames() << "vertex" << "color",

                             const ShaderInfo::ShaderVariablesNames & uniformValues =
                                ShaderInfo::ShaderVariablesNames() << "mvp" << "viewportSize");

        void fillBuffers(const PointsInfo::ModelPoints & modelPoints);

        void glStatesEnable();
        void glStatesDisable();

        void bindAttributeArrays(QOpenGLShaderProgram * program);
        void bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);
    };
}

#endif // POINTSMODEL_H
