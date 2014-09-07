#include <cmath>

#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(PointsModel * points, AbstractModel * parent,
                         const ShaderInfo::ShaderFiles & shaderFiles,
                         const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                         const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModelWithPoints(points, parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        points->setParent(this);
    }

    void HeadModel::init(const QVector3D & size) {
        ModelInfo::VerticesVTPtr vertices = new ModelInfo::VerticesVT;
        
        GLfloat step = 2.0f / size.z();
        GLfloat stepTexture = 1.0f / size.z();

        GLfloat zCurrent = -1.0f;
        GLfloat zCurrentTexture = 0.0f;

        for (int i = 0; i != (int) size.z(); ++ i) {
            vertices->push_back(ModelInfo::VertexVT(-1.0f, -1.0f, zCurrent, 0.0f, 1.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(-1.0f, 1.0f, zCurrent, 0.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0f, 1.0f, zCurrent, 1.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0f, -1.0f, zCurrent, 1.0f, 1.0f, zCurrentTexture));

            zCurrent += step;
            zCurrentTexture += stepTexture;
        };

        _step = QVector3D(1.0f / size.x(), 1.0f / size.y(), 1.0f / size.z());

        ModelInfo::BuffersVT buffers;

        buffers.vertices = ModelInfo::VerticesVTPointer(vertices);
        
        fillBuffers<ModelInfo::BuffersVT>(buffers);
    }

    void HeadModel::rotate(const QVector3D & rotation, const qreal & speed) {
        AbstractModel::rotate(QVector3D(rotation.x(), rotation.z(), - rotation.y()), speed);
    }
    
    QMatrix4x4 HeadModel::model(Viewport::Viewport * viewport) {
        QMatrix4x4 model = AbstractModel::model();
        
        if (viewport) {
            model = viewport->modelVoxel(model);
        }
        
        return model;
    }

    QMatrix4x4 HeadModel::view(Viewport::Viewport * viewport) {
        return viewport->viewVoxel();
    }
    
    void HeadModel::drawingRoutine() {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount());
    }

    void HeadModel::glStatesEnable() {
        //glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void HeadModel::glStatesDisable() {
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
    }

    void HeadModel::bindAttributeArrays(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["tex"]);
        program->setAttributeBuffer(attributeArrays["tex"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void HeadModel::bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport) {
        program->setUniformValue(uniformValues["view"], view(viewport));
        program->setUniformValue(uniformValues["model"], model(viewport));
        program->setUniformValue(uniformValues["projection"], projection(viewport));
        program->setUniformValue(uniformValues["normalMatrix"], normalMatrix(viewport));
        program->setUniformValue(uniformValues["scale"], scaleMatrix());
        program->setUniformValue(uniformValues["step"], _step);
    }
}
