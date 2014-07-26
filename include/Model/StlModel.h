#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/AbstractModel.h"

namespace ModelInfo {
    class VertexVN {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;

        VertexVN() { }
        VertexVN(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat nx,
                const GLfloat ny,
                const GLfloat nz
                ) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
    };

    using VerticesVN = QVector<VertexVN>;
    using VerticesVNPtr = VerticesVN *;

    using VerticesVNPointer = QSharedPointer<VerticesVN>;

    class BuffersVN : public BuffersV {
    public:
        VerticesVNPointer vertices;

        BuffersVN() { }
        BuffersVN(const VerticesVNPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

namespace Model {
    class StlModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit StlModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/stlVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/stlFragment.glsl")
                              ));

        void initShaderVariables(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort);
        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);

        ModelInfo::ViewAxisRange correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange);

        void glStatesEnable();
        void glStatesDisable();

        bool checkDepthBuffer(ViewPort::ViewPort & viewPort);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderNormal;

        // assume that this have uniform color
        ShaderInfo::ShaderVariable _shaderColorU;

        ShaderInfo::ShaderVariable _shaderMPV;
        ShaderInfo::ShaderVariable _shaderNormalMatrix;
    };
}
#endif // STLMODEL_H
