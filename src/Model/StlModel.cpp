#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {
    }

    void StlModel::initModel() {
        _shaderVertex = _program->attributeLocation("vertex");
        _shaderNormal = _program->attributeLocation("normal");

        _program->enableAttributeArray(_shaderVertex);
        _program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);

        _program->enableAttributeArray(_shaderNormal);
        _program->setAttributeBuffer(_shaderNormal, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    }

    void StlModel::initShaderVariables() {
        _shaderColorU = _program->uniformLocation("color");
        _shaderMPV = _program->uniformLocation("mvp");
        _shaderNormalMatrix = _program->uniformLocation("normalMatrix");
    }

    void StlModel::setShaderVariables(ViewPort::ViewPort & viewPort) {
        _program->setUniformValue(_shaderColorU, QVector4D(1.0, 1.0, 1.0, 1.0));
        _program->setUniformValue(_shaderMPV, viewPort.model() * viewPort.projection() * viewPort.view());
        _program->setUniformValue(_shaderNormalMatrix, viewPort.normalM());
    }
}
