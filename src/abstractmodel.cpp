#include "abstractmodel.h"

AbstractModel::AbstractModel() :
    _vboVert(QOpenGLBuffer::VertexBuffer),
    _vboInd(QOpenGLBuffer::IndexBuffer),
    _indexCount(0),
    _vertexCount(0) {

    _materialModel.emmisive = QVector4D(0.1, 0.1, 0.1, 0.1);
    _materialModel.diffuse = QVector4D(0.1, 0.1, 0.1, 0.1);
    _materialModel.specular = QVector4D(0.5, 0.7, 0.8, 1.0);

    _materialModel.shininess = 50.0;
}

AbstractModel::~AbstractModel() {
    reset();
}

void AbstractModel::reset() {
    _vao.destroy();

    _vboVert.destroy();
    _vboInd.destroy();
}

void AbstractModel::initMaterial(const ModelInfo::Material & material) {
    _materialModel = material;
}

void AbstractModel::drawModelWithIndices(QOpenGLShaderProgram * program) {
    setShaderVariables(program);

    _vao.bind();

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);

    _vao.release();
}

void AbstractModel::drawModelWithoutIndices(QOpenGLShaderProgram * program) {
    setShaderVariables(program);

    _vao.bind();

    glDrawArrays(GL_TRIANGLES, 0, _vertexCount);

    _vao.release();
}

void AbstractModel::setShaderVariables(QOpenGLShaderProgram * program) {
    program->setUniformValue(_shaderMaterialModelEmmisive, _materialModel.emmisive);
    program->setUniformValue(_shaderMaterialModelDiffuse, _materialModel.diffuse);
    program->setUniformValue(_shaderMaterialModelSpecular, _materialModel.specular);
    program->setUniformValue(_shaderMaterialModelShininess, _materialModel.shininess);
}

void AbstractModel::createModel(QOpenGLShaderProgram * program, ModelInfo::ModelBuffers & modelBuffers) {
    _vao.create();
    _vao.bind();

    _vboVert.create();
    _vboVert.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    _vertexCount = modelBuffers.vertices.data()->size();

    _vboVert.bind();
    _vboVert.allocate(modelBuffers.vertices.data(), _vertexCount * sizeof(modelBuffers.vertices->at(0)));

    if (modelBuffers.indices.data()) {
        _vboInd.create();
        _vboInd.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

        _indexCount = modelBuffers.indices.data()->size();

        _vboInd.bind();
        _vboInd.allocate(modelBuffers.indices.data(), _indexCount * sizeof(GLushort));

        modelBuffers.indices.clear();
    }

    initModel(program, modelBuffers);

    _vao.release();

    modelBuffers.vertices.clear();
}
