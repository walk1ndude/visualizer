#include <cmath>

#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(Scene::AbstractScene * scene, PointsModel * points, AbstractModel * parent,
                         const ShaderInfo::ShaderFiles & shaderFiles,
                         const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                         const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModelWithPoints(scene, points, parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        points->setParent(this);
    }

    void HeadModel::init(const TextureInfo::Size & size, const SliceInfo::PhysicalSize & physicalSize,
                         const TextureInfo::Scaling & scaling) {
        ModelInfo::VerticesVTPtr vertices = new ModelInfo::VerticesVT;
        ModelInfo::IndicesPtr indices = new ModelInfo::Indices;

        GLfloat scalingFactor = (GLfloat) scene()->scalingFactor();

        GLfloat zCurrent = - physicalSize.z() * scaling.z() / scalingFactor / 2.0f;
        
        GLfloat step = fabs(zCurrent * 2.0f) / size.z();
        GLfloat stepTexture = 1.0f / size.z();

        GLfloat zCurrentTexture = 0.0f;

        GLfloat w = physicalSize.x() / 2.0f * scaling.x() / scalingFactor;
        GLfloat h = physicalSize.y() / 2.0f * scaling.y() / scalingFactor;

        for (int i = 0; i != (int) size.z(); ++ i) {
            vertices->push_back(ModelInfo::VertexVT(- w, - h, zCurrent, 0.0f, 1.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(- w, h, zCurrent, 0.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(w, h, zCurrent, 1.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(w, - h, zCurrent, 1.0f, 1.0f, zCurrentTexture));

            indices->push_back(4 * i);
            indices->push_back(4 * i + 2);
            indices->push_back(4 * i + 1);
            indices->push_back(4 * i);
            indices->push_back(4 * i + 3);
            indices->push_back(4 * i + 2);

            zCurrent += step;
            zCurrentTexture += stepTexture;
        };

        _step = QVector3D(1.0f / size.x(), 1.0f / size.y(), 1.0f / size.z());

        ModelInfo::BuffersVT buffers;

        buffers.vertices = ModelInfo::VerticesVTPointer(vertices);
        buffers.indices = ModelInfo::IndicesPointer(indices);
        
        fillBuffers<ModelInfo::BuffersVT>(buffers);
    }

    void HeadModel::rotate(const QVector3D & rotation, const qreal & speed) {
        pointsModel()->rotate(QVector3D(rotation.x(), rotation.y(), - rotation.z()), speed);
        AbstractModel::rotate(QVector3D(rotation.x(), rotation.z(), - rotation.y()), speed);
    }
    
    QMatrix4x4 HeadModel::model(const Viewport::Viewport * viewport) const {
        QMatrix4x4 model = AbstractModel::model();
        
        if (viewport) {
            model = viewport->modelBillboard(model);
        }
        
        return model;
    }

    QMatrix4x4 HeadModel::view(const Viewport::Viewport * viewport) const {
        return viewport->viewBillboard();
    }
    
    void HeadModel::drawingRoutine() const {
        glDrawElements(GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0);
    }

    void HeadModel::glStatesEnable() const {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        AbstractModelWithPoints::glStatesEnable();
    }

    void HeadModel::glStatesDisable() const {
        glDisable(GL_BLEND);

        AbstractModelWithPoints::glStatesDisable();
    }

    void HeadModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["tex"]);
        program->setAttributeBuffer(attributeArrays["tex"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void HeadModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["view"], view(viewport));
        program->setUniformValue(uniformValues["model"], model(viewport));
        program->setUniformValue(uniformValues["projection"], projection(viewport));
        program->setUniformValue(uniformValues["normalMatrix"], normalMatrix(viewport));
        program->setUniformValue(uniformValues["scale"], scaleMatrix());
        program->setUniformValue(uniformValues["step"], _step);
    }
}
