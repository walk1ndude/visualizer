#include "Model/PointsModel.h"
#include "Model/HeadModel.h"
#include "Model/StlModel.h"

namespace Model {
    PointsModel::PointsModel(AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {
    }

    void PointsModel::fillBuffers(const PointsInfo::ModelPoints & modelPoints) {
        if (updateNeeded()) {
            ModelInfo::VerticesVCPPtr vertices = new ModelInfo::VerticesVCP;

            foreach (const PointsInfo::ModelPoint * modelPoint, modelPoints) {
                vertices->push_back(ModelInfo::VertexVCP(
                                        modelPoint->position.x(),
                                        modelPoint->position.y(),
                                        modelPoint->position.z(),
                                        modelPoint->color.redF(),
                                        modelPoint->color.greenF(),
                                        modelPoint->color.blueF(),
                                        modelPoint->polygonId
                                        ));
            }

            ModelInfo::BuffersVCP buffers;
            buffers.vertices = ModelInfo::VerticesVCPPointer(vertices);

            AbstractModel::fillBuffers<ModelInfo::BuffersVCP>(buffers, QOpenGLBuffer::DynamicDraw);

            modelUpdated();
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
        glDrawArrays(GL_POINTS, 0, vertexCount());
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

    void PointsModel::setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) {
        QMatrix4x4 model = parent()->model();

        if (dynamic_cast<Model::HeadModel *>(parent())) {
            model = viewPort.modelVoxel(model);
        }

        program->setUniformValue(_shaderMVP, viewPort.projection() * viewPort.view() * model);
    }
}
