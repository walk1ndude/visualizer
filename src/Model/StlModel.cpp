#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(Scene::AbstractScene * scene,
                       AbstractModel * parent,
                       const ShaderInfo::ShaderFiles & shaderFiles,
                       const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                       const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
            AbstractModelWithPoints(scene, parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {

    }

    void StlModel::init(const Params & params) { //(ModelInfo::BuffersVN buffers) {
        AbstractModelWithPoints::init(params);

        fillBuffers<ModelInfo::BuffersVN>(params["buffers"].value<ModelInfo::BuffersVN>());
    }

    void StlModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["normal"]);
        program->setAttributeBuffer(attributeArrays["normal"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void StlModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["colorU"], QVector4D(1.0, 1.0, 1.0, 1.0));

        program->setUniformValue(uniformValues["projection"], projection(viewport));
        program->setUniformValue(uniformValues["view"], view(viewport));
        program->setUniformValue(uniformValues["model"], model(viewport));
        
        program->setUniformValue(uniformValues["lightView"], viewport->modelBillboard());

        program->setUniformValue(uniformValues["eye"], viewport->eye());

        program->setUniformValue(uniformValues["normalMatrix"], normalMatrix(viewport));

        AbstractModelWithPoints::bindUniformValues(program, viewport);
    }
}

REGISTER_TYPE(StlModel)
