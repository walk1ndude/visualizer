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
        void setShaderVariables(Render::ViewPort & viewPort);

        void initModel(ModelInfo::BuffersV & buffers);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderNormal;

        // assume that this have uniform color
        ShaderInfo::ShaderVariable _shaderColorU;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
    };
}
#endif // STLMODEL_H
