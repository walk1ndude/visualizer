#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {
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

    void StlModel::initShaderVariables() {
        _shaderModel = _program->uniformLocation("color");

        _shaderModel = _program->uniformLocation("model");
        _shaderView = _program->uniformLocation("view");
        _shaderProjection = _program->uniformLocation("projection");
    }

    void StlModel::setShaderVariables(Render::ViewPort & viewPort) {
        _program->setUniformValue(_shaderColorU, QVector4D(1.0, 1.0, 1.0, 1.0));

        _program->setUniformValue(_shaderModel, viewPort.model());
        _program->setUniformValue(_shaderView, viewPort.view());
        _program->setUniformValue(_shaderProjection, viewPort.projection());
    }
}
