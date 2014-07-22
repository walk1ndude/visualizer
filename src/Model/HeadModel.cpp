#include <cmath>

#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {
        _scaleM.setToIdentity();
    }

    HeadModel::~HeadModel() {
        qDeleteAll(_facePoints.begin(), _facePoints.end());
    }

    void HeadModel::init(const int & depth) {
        _vertexCount = 4 * depth;
        _indexCount = 6 * depth;

        ModelInfo::VerticesVTPtr vertices = new ModelInfo::VerticesVT;
        ModelInfo::IndicesPtr indices = new ModelInfo::Indices;

        GLfloat step = 2.0 / (GLfloat) depth;
        GLfloat stepTexture = 1.0 / (GLfloat) depth;

        GLfloat zCurrent = -1.0;
        GLfloat zCurrentTexture = 0.0;

        for (int i = 0; i != depth; ++ i) {
            vertices->push_back(ModelInfo::VertexVT(-1.0, -1.0, zCurrent, 0.0, 1.0, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(-1.0, 1.0, zCurrent, 0.0, 0.0, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0, 1.0, zCurrent, 1.0, 0.0, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0, -1.0, zCurrent, 1.0, 1.0, zCurrentTexture));

            indices->push_back(4 * i);
            indices->push_back(4 * i + 2);
            indices->push_back(4 * i + 1);
            indices->push_back(4 * i);
            indices->push_back(4 * i + 3);
            indices->push_back(4 * i + 2);

            zCurrent += step;
            zCurrentTexture += stepTexture;
        };

        _step = 1.0 / depth;

        ModelInfo::BuffersVT buffers;

        buffers.vertices = ModelInfo::VerticesVTPointer(vertices);
        buffers.indices = ModelInfo::IndicesPointer(indices);

        initModel<ModelInfo::BuffersVT>(buffers);
    }

    void HeadModel::scale(const QVector3D & scale) {
        _scaleM.scale(scale);
    }

    void HeadModel::glStatesEnable() {
        glEnable(GL_CULL_FACE);

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

    ModelInfo::ViewAxisRange HeadModel::correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange) {
        return viewAxisRange;
    }

    void HeadModel::initShaderVariables() {
        _shaderVertex = _program->attributeLocation("vertex");
        _shaderTexHead = _program->attributeLocation("tex");

        _shaderView = _program->uniformLocation("view");
        _shaderModel = _program->uniformLocation("model");
        _shaderProjection = _program->uniformLocation("projection");
        _shaderNormalMatrix = _program->uniformLocation("normalMatrix");
        _shaderScale = _program->uniformLocation("scale");
        _shaderStep = _program->uniformLocation("stepSlices");
    }

    void HeadModel::bindShaderVariablesToBuffers() {
        _program->enableAttributeArray(_shaderVertex);
        _program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, sizeof(GLfloat) * 6);

        _program->enableAttributeArray(_shaderTexHead);
        _program->setAttributeBuffer(_shaderTexHead, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(GLfloat) * 6);
    }

    void HeadModel::setShaderVariables(ViewPort::ViewPort & viewPort) {
        _program->setUniformValue(_shaderView, viewPort.view());
        _program->setUniformValue(_shaderModel, _mMatrix);
        _program->setUniformValue(_shaderProjection, viewPort.projection());
        _program->setUniformValue(_shaderNormalMatrix, (_mMatrix * viewPort.view()).normalMatrix());
        _program->setUniformValue(_shaderScale, _scaleM);
        _program->setUniformValue(_shaderStep, _step);

        _facePointsProgram.setUniformValue(_program, _facePoints);
    }

    void HeadModel::addPoint(const QString & name, const PointsInfo::FacePoint & point, const ShaderInfo::ShaderVariableName & shaderVariableName) {
        if (_program) {
            _facePointsProgram.addPoint(_program, "facePoints." + shaderVariableName);
            _facePoints.insert(name, new PointsInfo::FacePoint(point.position, point.color));
        }
    }

    void HeadModel::checkDepthBuffer(ViewPort::ViewPort & viewPort) {
        QVector4D unprojectdPoint;

        foreach (PointsInfo::FacePoint * facePoint, _facePoints) {
            if (!facePoint->isPositionCalculated()) {
                if (!facePoint->depthTested()) {
                    GLfloat posZ;

                    glReadPixels(
                                (int) std::round(facePoint->position.x()),
                                (int) std::round(facePoint->position.y()),
                                1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &posZ
                    );

                    //qDebug() << posZ;

                    facePoint->position.setZ(posZ);
                    facePoint->depthTestCompleted();
                }

                if (viewPort.unproject(facePoint->position, unprojectdPoint)) {

                    facePoint->position = unprojectdPoint;
                    facePoint->positionCalculated();

                    qDebug() << facePoint->position;
                }
            }

        }
    }
}
