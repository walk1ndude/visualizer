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

    void StlModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["normal"]);
        program->setAttributeBuffer(attributeArrays["normal"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }
    
    void StlModel::rotate(const QVector3D & rotation, const qreal & speed) {
        AbstractModel::rotate(rotation);//QVector3D(rotation.x(), rotation.z(), rotation.x()), speed);
    }

    void StlModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["colorU"], QVector4D(1.0, 1.0, 1.0, 1.0));

        program->setUniformValue(uniformValues["projection"], projection(viewport));
        program->setUniformValue(uniformValues["view"], view(viewport));
        program->setUniformValue(uniformValues["model"], model(viewport));
        
        program->setUniformValue(uniformValues["lightView"], view(viewport));

        program->setUniformValue(uniformValues["eye"], viewport->eye());

        program->setUniformValue(uniformValues["normalMatrix"], normalMatrix(viewport));

        AbstractModelWithPoints::bindUniformValues(program, viewport);
    }
}
