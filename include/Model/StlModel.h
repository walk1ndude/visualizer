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

        void initModel(ModelInfo::BuffersV & buffers);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderNormal;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
        ShaderInfo::ShaderVariable _shaderScale;
    };
}
#endif // STLMODEL_H
