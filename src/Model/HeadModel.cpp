#include <cmath>

#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(PointsModel * points, AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModelWithPoints(points, parent, shaderFiles) {
            points->setParent(this);
    }

    HeadModel::~HeadModel() {
        qDeleteAll(_facePoints.begin(), _facePoints.end());
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

    void HeadModel::scale(const QVector3D & scale) {
        _scaleM.scale(scale);
    }

    void HeadModel::rotate(const QVector3D & rotation) {
        AbstractModel::rotate(QVector3D(rotation.x(), rotation.z(), - rotation.y()));
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
    
    void HeadModel::drawModelWithoutIndices() {
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

    void HeadModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderTexHead = program->attributeLocation("tex");

        _shaderView = program->uniformLocation("view");
        _shaderModel = program->uniformLocation("model");
        _shaderProjection = program->uniformLocation("projection");
        _shaderNormalMatrix = program->uniformLocation("normalMatrix");
        _shaderScale = program->uniformLocation("scale");
        _shaderStep = program->uniformLocation("stepSlices");
        
        AbstractModelWithPoints::initShaderVariables(program);
    }

    void HeadModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderTexHead);
        program->setAttributeBuffer(_shaderTexHead, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void HeadModel::setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport) {
        program->setUniformValue(_shaderView, view(viewport));
        program->setUniformValue(_shaderModel, model(viewport));
        program->setUniformValue(_shaderProjection, projection(viewport));
        program->setUniformValue(_shaderNormalMatrix, normalMatrix(viewport));
        program->setUniformValue(_shaderScale, _scaleM);
        program->setUniformValue(_shaderStep, _step);
    }
}
