#ifndef STLMODEL_H
#define STLMODEL_H

#include "Model/AbstractModelWithPoints.h"

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
                )  :
            x(x), y(y), z(z),
            nx(nx), ny(ny), nz(nz) {
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

Q_DECLARE_METATYPE(ModelInfo::BuffersVN)

namespace Model {
    class StlModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit StlModel(PointsModel * points = nullptr,
                          AbstractModel * parent = nullptr,
                          const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/stlVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/stlFragment.glsl")
                    << ShaderInfo::FragmentShaderFile(":shaders/helpersFragment.glsl")
                              ));

    protected:
        void initShaderVariables(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewPort);
        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();

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
