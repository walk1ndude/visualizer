#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModel.h"

// extend vertex types, add type which stores x,y,z; r,g,b and polygon id
namespace ModelInfo {
    class VertexVCP {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat p;

        VertexVCP() { }
        VertexVCP(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat r,
                const GLfloat g,
                const GLfloat b,
                const GLfloat p
                ) :
            x(x), y(y), z(z),
            r(r), g(g), b(b), p(p) {
        }
    };

    using VerticesVCP = QVector<VertexVCP>;
    using VerticesVCPPtr = VerticesVCP *;

    using VerticesVCPPointer = QSharedPointer<VerticesVCP>;

    class BuffersVCP : public BuffersV {
    public:
        VerticesVCPPointer vertices;

        BuffersVCP() { }
        BuffersVCP(const VerticesVCPPointer & vertices,
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
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewPort);

    protected:
        virtual void drawModelWithoutIndices();

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderColor;
        ShaderInfo::ShaderVariable _shaderPolygon;

        PointsInfo::ModelPoints _modelPoints;

        ShaderInfo::ShaderVariable _shaderMVP;
    };
}

#endif // POINTSMODEL_H
