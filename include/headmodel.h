#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

class HeadModel : protected QOpenGLFunctions_3_3_Core {

public:
    HeadModel();
    ~HeadModel();

    void init(QOpenGLShaderProgram * program, const int & zCount);
    void drawModel();

    void destroyModel();

private:
    QOpenGLBuffer _vboVert;
    QOpenGLBuffer _vboInd;

    QOpenGLVertexArrayObject _vao;

    int _shaderVertex;
    int _shaderTex;
    int _shaderNormal;

    int _indexCount;

    void initGeometry(QOpenGLShaderProgram * program, const int & zCount);
};

#endif // GEOMETRYENGINE_H
