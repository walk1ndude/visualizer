#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Model/AbstractModel.h"
#include "Model/VertexVC.h"

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
                ));

        void fillBuffers(const PointsInfo::ModelPoints & modelPoints);

        void glStatesEnable();
        void glStatesDisable();

        void initShaderVariables(QOpenGLShaderProgram * program);

        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderColor;

        ShaderInfo::ShaderVariable _shaderViewportSize;

        ShaderInfo::ShaderVariable _shaderMVP;
    };
}

#endif // POINTSMODEL_H
