#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel() :
        AbstractModel() {

    }

    void StlModel::initModel(ModelInfo::BuffersV & buffers) {
        _shaderVertex = _program->attributeLocation("vertex");
        _shaderNormal = _program->attributeLocation("normal");

        int offset = 0;

        _program->enableAttributeArray(_shaderVertex);
        _program->setAttributeBuffer(_shaderVertex, GL_FLOAT, offset, 3, sizeof(buffers.vertices->at(0)));

        offset += sizeof(GLfloat) * 3;

        _program->enableAttributeArray(_shaderNormal);
        _program->setAttributeBuffer(_shaderNormal, GL_FLOAT, offset, 3, sizeof(buffers.vertices->at(0)));
    }

    void StlModel::setShaderVariables() {

    }
}
