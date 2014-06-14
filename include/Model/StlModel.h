#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/Abstractmodel.h"

namespace Model {
    class StlModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit StlModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFile(":shaders/stlVertex.glsl"),
                              ShaderInfo::FramentShaderFile(":shaders/stlFragment.glsl")
                              ));

        void initShaderVariables();
        void setShaderVariables(ViewPort::ViewPort & viewPort);
        void bindShaderVariablesToBuffers();

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
