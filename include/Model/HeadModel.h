#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Model/Abstractmodel.h"

namespace Model {
    class HeadModel : public AbstractModel {

    public:
        explicit HeadModel();

        void init(const int & depth);
        void initModel(ModelInfo::BuffersV & buffers);

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
