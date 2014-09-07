#include "Model/EvaluatorModel.h"

namespace Model {
    EvaluatorModel::EvaluatorModel(AbstractModel * parent,
                                   const ShaderInfo::ShaderFiles & shaderFiles,
                                   const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                   const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {

    }

    void EvaluatorModel::setSize(const QSize & size) {
        setSize(size.width(), size.height());
    }

    void EvaluatorModel::setSize(const int & width, const int & height) {

    }

    void EvaluatorModel::setColor(const QVector4D & color) {

    }
}
