#include "glheadmodel.h"

GLHeadModel::GLHeadModel() :
    _vboVert(QOpenGLBuffer::VertexBuffer),
    _vboInd(QOpenGLBuffer::IndexBuffer),
    _vertices(0),
    _indices(0) {

}

GLHeadModel::~GLHeadModel() {
    _vboVert.destroy();
    _vboInd.destroy();

    delete [] _vertices;
    delete [] _indices;
}

void GLHeadModel::init(QOpenGLShaderProgram * program, const int & zCount) {
    initializeOpenGLFunctions();

    _shaderVertex = program->attributeLocation("vertex");
    _shaderTex = program->attributeLocation("tex");
    _shaderNormal = program->attributeLocation("normal");

    initGeometry(zCount);
}

void GLHeadModel::initGeometry(const int & zCount) {
    _vao.create();
    _vao.bind();

    _vboVert.create();
    _vboVert.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    int vertexCount = 4 * zCount;
    int indexCount = 6 * zCount;

    _vertices = new VertexData[vertexCount];
    _indices = new GLushort[indexCount];

    float step = 2.0 / (float) zCount;
    float stepTexture = 1.0 / (float) zCount;

    float zCurrent = -1.0;
    float zCurrentTexture = 0.0;

    int currentVert = 0;
    int currentIndex = 0;

    for (int i = 0; i != zCount; ++ i) {
        // this, or cv::flip in dicomreader.. anyway, we can always rotate to appropriate degree...
        _vertices[currentVert ++] = {QVector3D(-1.0, -1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, zCurrentTexture)};
        _vertices[currentVert ++] = {QVector3D(-1.0, 1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, zCurrentTexture)};
        _vertices[currentVert ++] = {QVector3D(1.0, 1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 0.0, zCurrentTexture)};
        _vertices[currentVert ++] = {QVector3D(1.0, -1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 1.0, zCurrentTexture)};

        _indices[currentIndex ++] = 4 * i;
        _indices[currentIndex ++] = 4 * i + 2;
        _indices[currentIndex ++] = 4 * i + 1;
        _indices[currentIndex ++] = 4 * i;
        _indices[currentIndex ++] = 4 * i + 3;
        _indices[currentIndex ++] = 4 * i + 2;

        zCurrent += step;
        zCurrentTexture += stepTexture;
    };

    _vboVert.bind();
    _vboVert.allocate(_vertices, vertexCount * sizeof(VertexData));

    _vboInd.create();
    _vboInd.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    _vboInd.bind();
    _vboInd.allocate(_indices, indexCount * sizeof(GLushort));

    _indexCount = indexCount;
}

void GLHeadModel::drawModel(QOpenGLShaderProgram * program) {
    int offset = 0;

    program->enableAttributeArray(_shaderVertex);
    program->setAttributeBuffer(_shaderVertex, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    program->enableAttributeArray(_shaderNormal);
    program->setAttributeBuffer(_shaderNormal, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    program->enableAttributeArray(_shaderTex);
    program->setAttributeBuffer(_shaderTex, GL_FLOAT, offset, 3, sizeof(VertexData));

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);
}
