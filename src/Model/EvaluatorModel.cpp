#include "Model/EvaluatorModel.h"

namespace Model {
    EvaluatorModel::EvaluatorModel(AbstractModel * parent,
                                   const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {

    }

    void EvaluatorModel::resizeMesh(const QSize & size) {
        resizeMesh(size.width(), size.height());
    }

    void EvaluatorModel::resizeMesh(const int & width, const int & height) {

    }
}
