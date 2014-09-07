#include "Model/EvaluatorModel.h"

#include <cmath>

namespace Model {
    EvaluatorModel::EvaluatorModel(AbstractModel * parent,
                                   const ShaderInfo::ShaderFiles & shaderFiles,
                                   const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                   const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {

    }

    void EvaluatorModel::setSize(const QSize & size) {
        setSize(size.width(), size.height());
    }

    void EvaluatorModel::setSize(const int & width, const int & height) {
        int w = abs(width);
        int h = abs(height);

        _width = w % 2 ? w + 1 : w;
        _height = h % 2 ? h + 1 : h;
    }

    void EvaluatorModel::setColor(const QVector3D & color) {
        _color = color;
    }

    void EvaluatorModel::setStep(const qreal & stepX, const qreal & stepY) {
        _stepX = fabs(stepX);
        _stepY = stepY ? fabs(stepY) : stepX;
    }

    void EvaluatorModel::init() {
        ModelInfo::VerticesVPtr vertices = new ModelInfo::VerticesV;

        GLfloat xLeft = - _width / 2 * _stepX;
        GLfloat yTop = - _height / 2 * _stepY;

        GLfloat xCur = xLeft;
        GLfloat yCur = yTop;

        for (int i = 0; i <= _height; ++ i) {
            vertices->push_back(ModelInfo::VertexV(xLeft, yCur, 0.0f));
            vertices->push_back(ModelInfo::VertexV(- xLeft, yCur, 0.0f));

            yCur += _stepY;
        }

        for (int i = 0; i <= _width; ++ i) {
            vertices->push_back(ModelInfo::VertexV(xCur, yTop, 0.0f));
            vertices->push_back(ModelInfo::VertexV(xCur, - yTop, 0.0f));

            xCur += _stepX;
        }

        /*
        for (auto vertex : *vertices) {
            qDebug() << vertex.x << vertex.y << vertex.z;
        }
        */

        ModelInfo::BuffersV buffers;

        buffers.vertices = ModelInfo::VerticesVPointer(vertices);

        AbstractModel::fillBuffers<ModelInfo::BuffersV>(buffers);
    }

    void EvaluatorModel::drawingRoutine() {
        glDrawArrays(GL_LINES, 0, vertexCount());
    }

    void EvaluatorModel::bindAttributeArrays(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());
    }

    void EvaluatorModel::bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport) {
        program->setUniformValue(uniformValues["vp"], projection(viewport) * viewport->viewBillboard());
        program->setUniformValue(uniformValues["color"], QVector4D(_color, 1.0f));
    }

    void EvaluatorModel::glStatesEnable() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void EvaluatorModel::glStatesDisable() {
        glDisable(GL_DEPTH_TEST);
    }
}
