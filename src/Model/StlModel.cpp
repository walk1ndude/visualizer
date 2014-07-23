#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {
    }

    void StlModel::initShaderVariables() {
        _shaderVertex = _program->attributeLocation("vertex");
        _shaderNormal = _program->attributeLocation("normal");

        _shaderColorU = _program->uniformLocation("colorU");
        _shaderMPV = _program->uniformLocation("mvp");
        _shaderNormalMatrix = _program->uniformLocation("normalMatrix");
    }

    void StlModel::glStatesEnable() {
        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void StlModel::glStatesDisable() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
    }

    ModelInfo::ViewAxisRange StlModel::correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange) {
        return viewAxisRange;
    }

    void StlModel::bindShaderVariablesToBuffers() {
        _program->enableAttributeArray(_shaderVertex);
        _program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);

        _program->enableAttributeArray(_shaderNormal);
        _program->setAttributeBuffer(_shaderNormal, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    }

    void StlModel::setShaderVariables(ViewPort::ViewPort & viewPort) {
        _program->setUniformValue(_shaderColorU, QVector4D(1.0, 1.0, 1.0, 1.0));
        _program->setUniformValue(_shaderMPV, viewPort.projection() * viewPort.view() * _mMatrix);
        _program->setUniformValue(_shaderNormalMatrix, (_mMatrix * viewPort.view()).normalMatrix());
    }

    bool StlModel::checkDepthBuffer(ViewPort::ViewPort & viewPort) {
        Q_UNUSED(viewPort)

        return false;
    }
}
