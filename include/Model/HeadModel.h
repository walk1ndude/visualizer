#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVT.h"

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

        virtual void rotate(const QVector3D & rotation, const qreal & speed = 0.5);
        
        virtual QMatrix4x4 model(Viewport::Viewport * viewport = nullptr);
        virtual QMatrix4x4 view(Viewport::Viewport * viewport);

    protected:
        void initShaderVariables(QOpenGLShaderProgram * program);
        void setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);
        void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();
        
        virtual void drawingRoutine();

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

        QVector3D _step;
    };
}
#endif // HEADMODEL_H
