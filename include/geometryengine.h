#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>

typedef struct _VertexData {
    QVector3D position;
    QVector3D normal;
    QVector3D texCoord;
}VertexData;

class GeometryEngine : protected QOpenGLFunctions {

public:
    GeometryEngine();
    ~GeometryEngine();

    void init(QOpenGLShaderProgram * program, const int & count);
    void drawModel(QOpenGLShaderProgram * program);

private:
    QOpenGLBuffer _vboVert;
    QOpenGLBuffer _vboInd;

    VertexData * _vertices;
    GLushort * _indices;

    int _shaderVertex;
    int _shaderTex;
    int _shaderNormal;

    int _indexCount;

    void initGeometry(const int & count);
};

#endif // GEOMETRYENGINE_H
