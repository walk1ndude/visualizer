#include "Model/StlModel.h"

namespace Model {
    StlModel::StlModel(Scene::AbstractScene * scene,
                       PointsModel * points, AbstractModel * parent,
                       const ShaderInfo::ShaderFiles & shaderFiles,
                       const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                       const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
            AbstractModelWithPoints(scene, points, parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
            points->setParent(this);
    }

    void StlModel::init(ModelInfo::BuffersVN buffers) {
        fillBuffers<ModelInfo::BuffersVN>(buffers);
    }

    void StlModel::glStatesEnable() {
        //glEnable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void StlModel::glStatesDisable() {
        //glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
    }

    void StlModel::bindAttributeArrays(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["normal"]);
        program->setAttributeBuffer(attributeArrays["normal"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void StlModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) {
        program->setUniformValue(uniformValues["colorU"], QVector4D(1.0, 1.0, 1.0, 1.0));
        program->setUniformValue(uniformValues["mvp"], projection(viewport) * view(viewport) * model(viewport));
        program->setUniformValue(uniformValues["normalMatrix"], normalMatrix(viewport));

        AbstractModelWithPoints::bindUniformValues(program, viewport);
    }
}
