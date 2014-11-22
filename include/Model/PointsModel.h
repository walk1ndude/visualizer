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
                                                     ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Points/vertex.glsl"),
                                                     ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Points/fragment.glsl"),
                                                     ShaderInfo::GeometryShaderFiles() << ShaderInfo::GeometryShaderFile(":shaders/Points/geometry.glsl")
                                                     ),

                             const ShaderInfo::ShaderVariablesNames & attributeArrays =
                             ShaderInfo::ShaderVariablesNames() << "vertex" << "color",

                             const ShaderInfo::ShaderVariablesNames & uniformValues =
                             ShaderInfo::ShaderVariablesNames() << "mvp" << "viewportSize");

        virtual bool hasDepth() const;

        virtual AbstractModel * invoke(const QString & name, const QVariantMap & params);

    protected:
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;

    private:
        void init(PointsInfo::ModelPoints * modelPoints);
    };
}

#endif // POINTSMODEL_H
