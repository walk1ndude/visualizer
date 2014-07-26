#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModel.h"

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
                ) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
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
    class HeadModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit HeadModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/sliceVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/sliceFragment.glsl")
                              ));

        ~HeadModel();

        void initShaderVariables();
        void setShaderVariables(ViewPort::ViewPort & viewPort);
        void bindShaderVariablesToBuffers();

        void glStatesEnable();
        void glStatesDisable();

        ModelInfo::ViewAxisRange correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange);

        void init(const QVector3D & size);

        void scale(const QVector3D & scale);

        void addPoint(
                const QString & name,
                const PointsInfo::FacePoint & point,
                const ShaderInfo::ShaderVariableName & shaderVariableName
                );

        bool checkDepthBuffer(ViewPort::ViewPort & viewPort);

    private:
        ShaderInfo::ShaderVariable _shaderVertex;
        ShaderInfo::ShaderVariable _shaderTexHead;

        ShaderInfo::ShaderVariable _shaderModel;
        ShaderInfo::ShaderVariable _shaderView;
        ShaderInfo::ShaderVariable _shaderProjection;
        ShaderInfo::ShaderVariable _shaderNormalMatrix;
        ShaderInfo::ShaderVariable _shaderScale;
        ShaderInfo::ShaderVariable _shaderStep;

        PointsInfo::FacePoints _facePoints;
        PointsInfo::FacePointsProgram _facePointsProgram;

        QMatrix4x4 _scaleM;

        QVector3D _step;
    };
}
#endif // HEADMODEL_H
