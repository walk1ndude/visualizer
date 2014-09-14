#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVT.h"

namespace Model {
    class HeadModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit HeadModel(Scene::AbstractScene * scene,
                           PointsModel * points = nullptr,
                           AbstractModel * parent = nullptr,

                           const ShaderInfo::ShaderFiles & shaderFiles =
                           ShaderInfo::ShaderFiles(
                              ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/sliceVertex.glsl"),
                              ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/sliceFragment.glsl")
                              << ShaderInfo::FragmentShaderFile(":shaders/helpersFragment.glsl")
                           ),

                           const ShaderInfo::ShaderVariablesNames & attributeArrays =
                           ShaderInfo::ShaderVariablesNames() << "vertex" << "tex",

                           const ShaderInfo::ShaderVariablesNames & uniformValues =
                           ShaderInfo::ShaderVariablesNames() << "view" << "model" << "projection" << "normalMatrix" << "scale" << "stepSlices");

        void init(const QVector3D & size);

        virtual void rotate(const QVector3D & rotation, const qreal & speed = 0.5);
        
        virtual QMatrix4x4 model(Viewport::Viewport * viewport = nullptr);
        virtual QMatrix4x4 view(Viewport::Viewport * viewport);

    protected:
        void bindUniformValues(QOpenGLShaderProgram * program, Viewport::Viewport * viewport);
        void bindAttributeArrays(QOpenGLShaderProgram * program);

        void glStatesEnable();
        void glStatesDisable();
        
        virtual void drawingRoutine();

    private:
        QVector3D _step;
    };
}
#endif // HEADMODEL_H
