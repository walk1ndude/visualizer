#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Model/AbstractModel.h"

namespace Model {
    class PointsModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit PointsModel(Scene::AbstractScene * scene, AbstractModel * parent = nullptr,
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

        void init(PointsInfo::ModelPoints * modelPoints);

    protected:
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;
    };
}

#endif // POINTSMODEL_H
