#include "glheadmodel.h"

typedef struct _VertexData {
    QVector3D position;
    QVector3D normal;
    QVector3D texCoord;
}VertexData;

GLHeadModel::GLHeadModel() :
    _vboVert(QOpenGLBuffer::VertexBuffer),
    _vboInd(QOpenGLBuffer::IndexBuffer) {

}

GLHeadModel::~GLHeadModel() {
    _vboVert.destroy();
    _vboInd.destroy();
}

void GLHeadModel::init(QOpenGLShaderProgram * program, const int & zCount) {
    initializeOpenGLFunctions();

    _shaderVertex = program->attributeLocation("vertex");
    _shaderTex = program->attributeLocation("tex");
    _shaderNormal = program->attributeLocation("normal");

    initGeometry(program, zCount);
}

void GLHeadModel::initGeometry(QOpenGLShaderProgram * program, const int & zCount) {
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
        // this, or cv::flip in dicomreader.. anyway, we can always rotate to appropriate degree...
        vertices[currentVert ++] = {QVector3D(-1.0, -1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(-1.0, 1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(1.0, 1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 0.0, zCurrentTexture)};
        vertices[currentVert ++] = {QVector3D(1.0, -1.0,  zCurrent), QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 1.0, zCurrentTexture)};

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

    program->enableAttributeArray(_shaderNormal);
    program->setAttributeBuffer(_shaderNormal, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    program->enableAttributeArray(_shaderTex);
    program->setAttributeBuffer(_shaderTex, GL_FLOAT, offset, 3, sizeof(VertexData));

    _vboInd.create();
    _vboInd.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    _vboInd.bind();
    _vboInd.allocate(indices, indexCount * sizeof(GLushort));

    _indexCount = indexCount;

    _vao.release();

    delete [] vertices;
    delete [] indices;
}

void GLHeadModel::drawModel() {
    _vao.bind();

    glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);

    qDebug() << glGetError();

    _vao.release();
}
