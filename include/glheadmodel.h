#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

typedef struct _VertexData {
    QVector3D position;
    QVector3D normal;
    QVector3D texCoord;
}VertexData;

class GLHeadModel : protected QOpenGLFunctions_3_3_Core {

public:
    GLHeadModel();
    ~GLHeadModel();

    void init(QOpenGLShaderProgram * program, const int & zCount, QOpenGLFunctions_3_3_Core * openglFuncs);
    void drawModel(QOpenGLShaderProgram * program);

private:
    QOpenGLBuffer _vboVert;
    QOpenGLBuffer _vboInd;

    QOpenGLVertexArrayObject _vao;

    QOpenGLFunctions_3_3_Core * _openglFuncs;

    VertexData * _vertices;
    GLushort * _indices;

    int _shaderVertex;
    int _shaderTex;
    int _shaderNormal;

    int _indexCount;

    void initGeometry(const int & zCount);
};

#endif // GEOMETRYENGINE_H
