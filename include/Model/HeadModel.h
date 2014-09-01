#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModelWithPoints.h"

namespace ModelInfo {
    class VertexVT {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat tx;
        GLfloat ty;
        GLfloat tz;

        VertexVT() { }
        VertexVT(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat tx,
                const GLfloat ty,
                const GLfloat tz
                ) :
            x(x), y(y), z(z),
            tx(tx), ty(ty), tz(tz) {
        }
    };

    using VerticesVT = QVector<VertexVT>;
    using VerticesVTPtr = VerticesVT *;

    using VerticesVTPointer = QSharedPointer<VerticesVT>;

    class BuffersVT : public BuffersV {
    public:
        VerticesVTPointer vertices;

        BuffersVT() { }
        BuffersVT(const VerticesVTPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

namespace Model {
    class HeadModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit HeadModel(PointsModel * points = nullptr,
                           AbstractModel * parent = nullptr,
                           const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/sliceVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/sliceFragment.glsl")
                    << ShaderInfo::FragmentShaderFile(":shaders/helpersFragment.glsl")
                              ));

        ~HeadModel();

        void init(const QVector3D & size);

        void scale(const QVector3D & scale);

        virtual void rotate(const QVector3D & rotation);

    protected:
        void initShaderVariables(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewPort);
        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();
        
        virtual void drawModelWithIndices();

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderTexHead;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
        ShaderInfo::ShaderVariable _shaderNormalMatrix;
        ShaderInfo::ShaderVariable _shaderScale;
        ShaderInfo::ShaderVariable _shaderStep;

        PointsInfo::ModelPoints _facePoints;
        
        QMatrix4x4 _scaleM;

        QVector3D _step;
    };
}
#endif // HEADMODEL_H
