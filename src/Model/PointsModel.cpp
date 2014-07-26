#include "Model/PointsModel.h"

namespace Model {
    PointsModel::PointsModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {

    }

    void PointsModel::init(const PointsInfo::FacePoints & facePoints) {
        update(facePoints);
    }

    void PointsModel::update(const PointsInfo::FacePoints & facePoints) {
        ModelInfo::VerticesVCPPtr vertices = new ModelInfo::VerticesVCP;

        foreach (const PointsInfo::FacePoint * facePoint, facePoints) {
            vertices->push_back(ModelInfo::VertexVCP(
                                    facePoint->position.x(),
                                    facePoint->position.y(),
                                    facePoint->position.z(),
                                    facePoint->color.redF(),
                                    facePoint->color.greenF(),
                                    facePoint->color.blueF(),
                                    facePoint->polygonId
                                    ));
        }

        ModelInfo::BuffersVCP buffers;
        buffers.vertices = ModelInfo::VerticesVCPPointer(vertices);

        initModel<ModelInfo::BuffersVCP>(buffers, QOpenGLBuffer::DynamicDraw);
    }

    void PointsModel::glStatesEnable() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void PointsModel::glStatesDisable() {
        glDisable(GL_DEPTH_TEST);
    }

    void PointsModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderColor = program->attributeLocation("color");
        _shaderPolygon = program->attributeLocation("polygon");
    }

    void PointsModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderColor);
        program->setAttributeArray(_shaderColor, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());

        program->enableAttributeArray(_shaderPolygon);
        program->setAttributeValue(_shaderPolygon, GL_FLOAT, sizeof(GLfloat), 1, stride());
    }
}
