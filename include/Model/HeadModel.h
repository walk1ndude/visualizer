#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Model/Abstractmodel.h"

namespace Model {
    class HeadModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit HeadModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFile(":shaders/sliceVertex.glsl"),
                              ShaderInfo::FramentShaderFile(":shaders/sliceFragment.glsl")
                              ));

        void initShaderVariables();
        void setShaderVariables(ViewPort::ViewPort & viewPort);

        void init(const int & depth);
        void initModel();

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderTexHead;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
        ShaderInfo::ShaderVariable _shaderScale;
        ShaderInfo::ShaderVariable _shaderStep;
    };
}
#endif // HEADMODEL_H
