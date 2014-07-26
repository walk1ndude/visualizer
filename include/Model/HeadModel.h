#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Model/AbstractModel.h"

#include "Info/PointsInfo.h"

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
