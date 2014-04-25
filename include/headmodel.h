#ifndef HEADMODEL_H
#define HEADMODEL_H

#include <QtGui/QOpenGLFunctions_3_3_Core>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

typedef struct _Material {
    QVector4D emmisive;
    QVector4D diffuse;
    QVector4D specular;
    float shininess;
} Material;

class HeadModel : protected QOpenGLFunctions_3_3_Core {

public:
    HeadModel();
    ~HeadModel();

    void init(QOpenGLShaderProgram * program, const int & zCount);
    void drawModel(QOpenGLShaderProgram * program);

    void destroyModel();

private:
    QOpenGLBuffer _vboVert;
    QOpenGLBuffer _vboInd;

    Material _headMaterial;

    QOpenGLVertexArrayObject _vao;

    int _shaderVertex;
    int _shaderTex;

    int _shaderHeadMaterialEmmisive;
    int _shaderHeadMaterialDiffuse;
    int _shaderHeadMaterialSpecular;
    int _shaderHeadMaterialShininess;

    int _indexCount;

    void initGeometry(QOpenGLShaderProgram * program, const int & zCount);
};

#endif // HEADMODEL_H
