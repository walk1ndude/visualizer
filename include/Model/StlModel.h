#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/Abstractmodel.h"

namespace Model {
    class StlModel : public AbstractModel {

    public:
        explicit StlModel();

        virtual void initShaderVariables();

        void initModel(ModelInfo::BuffersV & buffers);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderNormal;
    };
}
#endif // STLMODEL_H
