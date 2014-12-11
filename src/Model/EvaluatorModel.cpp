#include "Model/EvaluatorModel.h"

#include <cmath>

namespace Model {
    EvaluatorModel::EvaluatorModel(Scene::AbstractScene * scene,
                                   const ShaderInfo::ShaderFiles & shaderFiles,
                                   const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                   const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(scene, shaderFiles, shaderAttributeArrays, shaderUniformValues) {

    }

    void EvaluatorModel::setSize(const QSize & size) {
        setSize(size.width(), size.height());
    }

    void EvaluatorModel::setSize(const int & width, const int & height) {
        int w = abs(width);
        int h = abs(height);

        _width = w;
        _height = h;
    }

    void EvaluatorModel::setColor(const QColor & color) {
        _color = color;
    }

    void EvaluatorModel::setStep(const qreal & stepX, const qreal & stepY) {
        _stepX = fabs(stepX);
        _stepY = stepY ? fabs(stepY) : stepX;
    }

    void EvaluatorModel::init(const ModelInfo::Params & params) {
        AbstractModel::init(params);

        setSize(params["width"].toInt(), params["height"].toInt());
        setStep(params["stepX"].toReal(), params["stepY"].toReal());
        setColor(params["color"].value<QColor>());

        ModelInfo::VerticesVPtr vertices = new ModelInfo::VerticesV;

        GLfloat scalingFactor = (GLfloat) (1.0f / scene()->scalingFactor());

        GLfloat xLeft = - _width * _stepX * scalingFactor;
        GLfloat yTop = - _height * _stepY * scalingFactor;

        GLfloat xCur = xLeft;
        GLfloat yCur = yTop;

        GLfloat stepX = _stepX * scalingFactor;
        GLfloat stepY = _stepY * scalingFactor;

        for (int i = 0; i <= 2 * _height; ++ i) {
            vertices->push_back(ModelInfo::VertexV(xLeft, yCur, 0.0f));
            vertices->push_back(ModelInfo::VertexV(- xLeft, yCur, 0.0f));

            yCur += stepY;
        }

        for (int i = 0; i <= 2 * _width; ++ i) {
            vertices->push_back(ModelInfo::VertexV(xCur, yTop, 0.0f));
            vertices->push_back(ModelInfo::VertexV(xCur, - yTop, 0.0f));

            xCur += stepX;
        }

        ModelInfo::BuffersV buffers;

        buffers.vertices = ModelInfo::VerticesVPointer(vertices);

        AbstractModel::fillBuffers<ModelInfo::BuffersV>(buffers);
    }

    void EvaluatorModel::drawingRoutine() const {
        glDrawArrays(GL_LINES, 0, vertexCount());
    }

    void EvaluatorModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        QMutexLocker locker (&modelMutex);

        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());
    }

    void EvaluatorModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["mvp"], projection(viewport) * view(viewport) * model(viewport));
        program->setUniformValue(uniformValues["color"], _color);
    }

    void EvaluatorModel::glStatesEnable() const {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        AbstractModel::glStatesEnable();
    }

    void EvaluatorModel::glStatesDisable() const {
        glDisable(GL_DEPTH_TEST);

        AbstractModel::glStatesDisable();
    }

    Camera::ModelMatrix EvaluatorModel::model(const Viewport::Viewport * viewport) const {
        return viewport->modelBillboard();
    }
}

REGISTER_TYPE(EvaluatorModel)
