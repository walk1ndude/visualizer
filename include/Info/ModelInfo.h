#ifndef MODELINFO_H
#define MODELINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace ModelInfo {
    class VertexV {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;

        VertexV() { }
        VertexV(GLfloat x,
                GLfloat y,
                GLfloat z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };

    using Indices = QVector<GLuint>;
    using IndicesPtr = Indices *;
    using IndicesPointer = QSharedPointer<Indices>;

    using VerticesV = QVector<VertexV>;
    using VerticesVPtr = VerticesV *;
    using VerticesVPointer = QSharedPointer<VerticesV>;

    class BuffersV {
    public:
        IndicesPointer indices;
        VerticesVPointer vertices;

        BuffersV() { }
        BuffersV(const VerticesVPointer & vertices,
                 const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    using ViewAxisRange = QVector2D;

    enum ViewAxis {
        XAXIS,
        YAXIS,
        ZAXIS
    };

    class ViewRange {
    private:
        ShaderInfo::ShaderVariable shaderXRange;
        ShaderInfo::ShaderVariable shaderYRange;
        ShaderInfo::ShaderVariable shaderZRange;
    public:
        ViewAxisRange xRange;
        ViewAxisRange yRange;
        ViewAxisRange zRange;

        ViewRange(const ViewAxisRange & xRange,
                  const ViewAxisRange & yRange,
                  const ViewAxisRange & zRange,
                  QOpenGLShaderProgram * program,
                  const ShaderInfo::ShaderVariablesNames & shaderVariables) {
            this->xRange = xRange;
            this->yRange = yRange;
            this->zRange = zRange;

            shaderXRange = program->uniformLocation(shaderVariables.at(0));
            shaderYRange = program->uniformLocation(shaderVariables.at(1));
            shaderZRange = program->uniformLocation(shaderVariables.at(2));
        }

        void setUniformValue(QOpenGLShaderProgram * program) {
            program->setUniformValue(shaderXRange, xRange);
            program->setUniformValue(shaderYRange, yRange);
            program->setUniformValue(shaderZRange, zRange);
        }

        void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                               const ModelInfo::ViewAxis viewAxis) {
            switch (viewAxis) {
            case ModelInfo::XAXIS:
                xRange = viewAxisRange;
                break;
            case ModelInfo::YAXIS:
                yRange = viewAxisRange;
                break;
            case ModelInfo::ZAXIS:
                zRange = viewAxisRange;
                break;
            }
        }
    };
}


#endif // MODELINFO_H
