#include <cmath>

#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {
    }

    HeadModel::~HeadModel() {
        qDeleteAll(_facePoints.begin(), _facePoints.end());
    }

    void HeadModel::init(const QVector3D & size) {
        ModelInfo::VerticesVTPtr vertices = new ModelInfo::VerticesVT;
        ModelInfo::IndicesPtr indices = new ModelInfo::Indices;

        GLfloat step = 2.0f / size.z();
        GLfloat stepTexture = 1.0f / size.z();

        GLfloat zCurrent = -1.0f;
        GLfloat zCurrentTexture = 0.0f;

        for (int i = 0; i != (int) size.z(); ++ i) {
            vertices->push_back(ModelInfo::VertexVT(-1.0f, -1.0f, zCurrent, 0.0f, 1.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(-1.0f, 1.0f, zCurrent, 0.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0f, 1.0f, zCurrent, 1.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(1.0f, -1.0f, zCurrent, 1.0f, 1.0f, zCurrentTexture));

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

    void HeadModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderTexHead = program->attributeLocation("tex");

        _shaderView = program->uniformLocation("view");
        _shaderModel = program->uniformLocation("model");
        _shaderProjection = program->uniformLocation("projection");
        _shaderNormalMatrix = program->uniformLocation("normalMatrix");
        _shaderScale = program->uniformLocation("scale");
        _shaderStep = program->uniformLocation("stepSlices");
    }

    void HeadModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderTexHead);
        program->setAttributeBuffer(_shaderTexHead, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void HeadModel::setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) {
        QMatrix4x4 modelMatrix = viewPort.modelVoxel(model());

        program->setUniformValue(_shaderView, viewPort.viewVoxel());
        program->setUniformValue(_shaderModel, modelMatrix);
        program->setUniformValue(_shaderProjection, viewPort.projection());
        program->setUniformValue(_shaderNormalMatrix, QMatrix4x4((modelMatrix * viewPort.viewVoxel()).normalMatrix()));
        program->setUniformValue(_shaderScale, _scaleM);
        program->setUniformValue(_shaderStep, _step);

        _facePointsProgram.setUniformValue(program, _facePoints, viewPort.projection() * viewPort.viewVoxel());
    }

    void HeadModel::addPoint(const QString & name, const PointsInfo::FacePoint & point, const ShaderInfo::ShaderVariableName & shaderVariableName) {
        /*
        if (_program) {
            _facePointsProgram.addPoint(_program, "facePoints." + shaderVariableName);
            _facePoints.insert(name, new PointsInfo::FacePoint(point.position, point.color));
        }*/
    }

    bool HeadModel::checkDepthBuffer(ViewPort::ViewPort & viewPort) {
        QVector4D unprojectdPoint;

        bool updateNeeded = false;

        foreach (PointsInfo::FacePoint * facePoint, _facePoints) {
            if (viewPort.pointInViewPort(facePoint->position) && !facePoint->isPositionCalculated()) {
                GLushort posZ;

                facePoint->position.setX(std::round(facePoint->position.x()));
                facePoint->position.setY(std::round(facePoint->position.y()));

                // usage of GL_UNSIGNED_SHORT explaned here http://www.opengl.org/wiki/Common_Mistakes#Depth_Buffer_Precision
                glReadPixels(
                            (int) facePoint->position.x(),
                            (int) facePoint->position.y(),
                            1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, &posZ
                            );

                facePoint->position.setZ(posZ / 65536.0f);

                //facePoint->position = viewPortCoords;

                if (viewPort.unproject(facePoint->position, unprojectdPoint)) {
                    facePoint->position = unprojectdPoint;
                    facePoint->positionCalculated();

                    updateNeeded = true;
                    qDebug() << facePoint->position;
                }
            }
        }

        return updateNeeded;
    }
}
