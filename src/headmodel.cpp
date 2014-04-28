#include "headmodel.h"

typedef struct _VertexData {
    QVector3D position;
    QVector3D texCoord;
} VertexData;

HeadModel::HeadModel() :
    _vboVert(QOpenGLBuffer::VertexBuffer),
    _vboInd(QOpenGLBuffer::IndexBuffer) {

    _headMaterial.emmisive = QVector4D(0.1, 0.1, 0.1, 0.1);
    _headMaterial.diffuse = QVector4D(0.1, 0.1, 0.1, 0.1);
    _headMaterial.specular = QVector4D(0.5, 0.7, 0.8, 1.0);

    _headMaterial.shininess = 50.0;
}

HeadModel::~HeadModel() {
    _vboVert.destroy();
    _vboInd.destroy();
}

void HeadModel::init(QOpenGLShaderProgram * program, const int & zCount) {
    initializeOpenGLFunctions();

    _shaderVertex = program->attributeLocation("vertex");
    _shaderTex = program->attributeLocation("tex");

    _shaderHeadMaterialEmmisive = program->uniformLocation("headMaterial.emmisive");
    _shaderHeadMaterialDiffuse = program->uniformLocation("headMaterial.diffuse");
    _shaderHeadMaterialSpecular = program->uniformLocation("headMaterial.specular");
    _shaderHeadMaterialShininess = program->uniformLocation("headMaterial.shininess");

    initGeometry(program, zCount);
}

void HeadModel::initGeometry(QOpenGLShaderProgram * program, const int & zCount) {
    _vao.create();
    _vao.bind();

    _vboVert.create();
    _vboVert.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    int vertexCount = 4 * zCount;
    int indexCount = 6 * zCount;

    VertexData * vertices = new VertexData[vertexCount];
    GLushort * indices = new GLushort[indexCount];

    float step = 2.0 / (float) zCount;
    float stepTexture = 1.0 / (float) zCount;

    float zCurrent = -1.0;
    float zCurrentTexture = 0.0;

    int currentVert = 0;
    int currentIndex = 0;

    for (int i = 0; i != zCount; ++ i) {
        vertices[currentVert ++] = {QVector3D(-1.0, -1.0,  zCurrent), QVector3D(0.0, 1.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(-1.0, 1.0,  zCurrent), QVector3D(0.0, 0.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(1.0, 1.0,  zCurrent), QVector3D(1.0, 0.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(1.0, -1.0,  zCurrent), QVector3D(1.0, 1.0, zCurrentTexture)};

        indices[currentIndex ++] = 4 * i;
        indices[currentIndex ++] = 4 * i + 2;
        indices[currentIndex ++] = 4 * i + 1;
        indices[currentIndex ++] = 4 * i;
        indices[currentIndex ++] = 4 * i + 3;
        indices[currentIndex ++] = 4 * i + 2;

        zCurrent += step;
        zCurrentTexture += stepTexture;
    };

    _vboVert.bind();
    _vboVert.allocate(vertices, vertexCount * sizeof(VertexData));

    int offset = 0;

    program->enableAttributeArray(_shaderVertex);
    program->setAttributeBuffer(_shaderVertex, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    program->enableAttributeArray(_shaderTex);
    program->setAttributeBuffer(_shaderTex, GL_FLOAT, offset, 3, sizeof(VertexData));

    _vboInd.create();
    _vboInd.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    _vboInd.bind();
    _vboInd.allocate(indices, indexCount * sizeof(GLushort));

    _indexCount = indexCount;

    _vao.release();

    _vboVert.release();
    _vboInd.release();

    delete [] vertices;
    delete [] indices;
}

void HeadModel::drawModel(QOpenGLShaderProgram * program) {
    program->setUniformValue(_shaderHeadMaterialEmmisive, _headMaterial.emmisive);
    program->setUniformValue(_shaderHeadMaterialDiffuse, _headMaterial.diffuse);
    program->setUniformValue(_shaderHeadMaterialSpecular, _headMaterial.specular);
    program->setUniformValue(_shaderHeadMaterialShininess, _headMaterial.shininess);

    _vao.bind();

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);

    _vao.release();
}

void HeadModel::destroyModel() {
    _vao.destroy();

    _vboVert.destroy();
    _vboInd.destroy();
}
