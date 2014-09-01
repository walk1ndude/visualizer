#include "Model/PointsModel.h"
#include "Model/HeadModel.h"
#include "Model/StlModel.h"

namespace Model {
    PointsModel::PointsModel(AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {

    }

    void PointsModel::fillBuffers(const PointsInfo::ModelPoints & modelPoints) {
        _modelPoints = modelPoints;

        if (updateNeeded()) {
            ModelInfo::VerticesVCPtr vertices = new ModelInfo::VerticesVC;

            for (const PointsInfo::ModelPoint * modelPoint : modelPoints) {
                for (int i = 0; i != 3; ++ i) {
                    vertices->push_back(ModelInfo::VertexVC(modelPoint->position.x(),
                                                            modelPoint->position.y(),
                                                            modelPoint->position.z(),
                                                            modelPoint->color.redF(),
                                                            modelPoint->color.greenF(),
                                                            modelPoint->color.blueF()
                                                            ));
                }
            }

            ModelInfo::BuffersVC buffers;
            buffers.vertices = ModelInfo::VerticesVCPointer(vertices);

            AbstractModel::fillBuffers<ModelInfo::BuffersVC>(buffers, QOpenGLBuffer::DynamicDraw);
        }
    }

    void PointsModel::glStatesEnable() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void PointsModel::glStatesDisable() {
        glDisable(GL_DEPTH_TEST);
    }
    
    void PointsModel::drawModelWithoutIndices() {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount());
    }

    void PointsModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderColor = program->attributeLocation("color");
        _shaderPolygon = program->attributeLocation("polygon");

        _shaderMVP = program->uniformLocation("mvp");
    }

    void PointsModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderColor);
        program->setAttributeBuffer(_shaderColor, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());

        program->enableAttributeArray(_shaderPolygon);
        program->setAttributeValue(_shaderPolygon, GL_FLOAT, sizeof(GLfloat), 1, stride());
    }

    void PointsModel::setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport) {
        program->setUniformValue(_shaderMVP, viewport->projection() * viewport->view() * parent()->model(viewport));
    }
}
