#include "stlmodel.h"

StlModel::StlModel() :
    AbstractModel() {

}

StlModel::~StlModel() {

}

void StlModel::initModel(QOpenGLShaderProgram * program, ModelInfo::ModelBuffers & modelBuffers) {
    _shaderMaterialModelEmmisive = program->uniformLocation("stlMaterial.emmisive");
    _shaderMaterialModelDiffuse = program->uniformLocation("stlMaterial.diffuse");
    _shaderMaterialModelSpecular = program->uniformLocation("stlMaterial.specular");
    _shaderMaterialModelShininess = program->uniformLocation("stlMaterial.shininess");

    _shaderVertex = program->attributeLocation("vertex");
    _shaderNormal = program->attributeLocation("normal");

    int offset = 0;

    program->enableAttributeArray(_shaderVertex);
    program->setAttributeBuffer(_shaderVertex, GL_FLOAT, offset, 3, sizeof(modelBuffers.vertices->at(0)));

    offset += sizeof(GLfloat) * 3;

    program->enableAttributeArray(_shaderNormal);
    program->setAttributeBuffer(_shaderNormal, GL_FLOAT, offset, 3, sizeof(modelBuffers.vertices->at(0)));
}

