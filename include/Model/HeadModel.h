#ifndef HEADMODEL_H
#define HEADMODEL_H

#include "Info/PointsInfo.h"
#include "Info/SliceInfo.h"

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

        void init(const TextureInfo::Size & size, const SliceInfo::PhysicalSize & physicalSize, const TextureInfo::Scaling & scaling);

        virtual void rotate(const QVector3D & rotation, const qreal & speed = 0.5);
        
        virtual QMatrix4x4 model(const Viewport::Viewport * viewport = nullptr) const;
        virtual QMatrix4x4 view(const Viewport::Viewport * viewport) const;

    protected:
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;
        
        virtual void drawingRoutine() const;

    private:
        QVector3D _step;
    };
}
#endif // HEADMODEL_H
