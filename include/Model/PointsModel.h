#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModel.h"

// extend vertex types, add type which stores x,y,z; r,g,b and polygon id
namespace ModelInfo {
    class VertexVC {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;

        VertexVC() { }
        VertexVC(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat r,
                const GLfloat g,
                const GLfloat b
                ) :
            x(x), y(y), z(z),
            r(r), g(g), b(b) {
        }
    };

    using VerticesVC = QVector<VertexVC>;
    using VerticesVCPtr = VerticesVC *;

    using VerticesVCPointer = QSharedPointer<VerticesVC>;

    class BuffersVC : public BuffersV {
    public:
        VerticesVCPointer vertices;

        BuffersVC() { }
        BuffersVC(const VerticesVCPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

namespace Model {
    class PointsModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit PointsModel(AbstractModel * parent = nullptr,
                             const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/pointsVertex.glsl"),
                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/pointsFragment.glsl"),
                    ShaderInfo::GeometryShaderFiles() << ShaderInfo::GeometryShaderFile(":shaders/pointsGeometry.glsl")
                ));

        void fillBuffers(const PointsInfo::ModelPoints & modelPoints);

        void glStatesEnable();
        void glStatesDisable();

        void initShaderVariables(QOpenGLShaderProgram * program);

        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderColor;

        ShaderInfo::ShaderVariable _shaderViewportSize;

        ShaderInfo::ShaderVariable _shaderMVP;
    };
}

#endif // POINTSMODEL_H
