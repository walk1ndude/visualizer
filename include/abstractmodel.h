#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>

#include "modelinfo.h"

class AbstractModel {

public:
    AbstractModel();
    ~AbstractModel();

    virtual void reset() final;

    virtual void initModel(QOpenGLShaderProgram * program, ModelInfo::ModelBuffers & modelBuffers) = 0;

    virtual void drawModelWithIndices(QOpenGLShaderProgram * program) final;
    virtual void drawModelWithoutIndices(QOpenGLShaderProgram * program) final;

    virtual void initMaterial(const ModelInfo::Material & material) final;

protected:
    QOpenGLBuffer _vboVert;
    QOpenGLBuffer _vboInd;

    QOpenGLVertexArrayObject _vao;

    ModelInfo::Material _materialModel;

    GLsizei _indexCount;
    GLsizei _vertexCount;

    int _shaderMaterialModelEmmisive;
    int _shaderMaterialModelDiffuse;
    int _shaderMaterialModelSpecular;
    int _shaderMaterialModelShininess;

    int _shaderVertex;

private:
    void setShaderVariables(QOpenGLShaderProgram * program);

public slots:
    virtual void createModel(QOpenGLShaderProgram * program, ModelInfo::ModelBuffers & modelBuffers) final;
};


#endif // ABSTRACTMODEL_H
