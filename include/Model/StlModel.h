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
                              ));

    protected:
        void initShaderVariables(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);
        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderNormal;

        // assume that this have uniform color
        ShaderInfo::ShaderVariable _shaderColorU;

        ShaderInfo::ShaderVariable _shaderMPV;
        ShaderInfo::ShaderVariable _shaderNormalMatrix;
    };
}
#endif // STLMODEL_H
