#ifndef VOLUMEMODEL_H
#define VOLUMEMODEL_H

#include "Info/PointsInfo.h"
#include "Info/VolumeInfo.h"

#include "Model/AbstractModelWithPoints.h"
#include "Model/VertexVT.h"

namespace Model {
    class VolumeModel : public AbstractModelWithPoints {
        Q_OBJECT
    public:
        explicit VolumeModel(Scene::AbstractScene * scene,
                             AbstractModel * parent = nullptr,

                           const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/Volume/vertex.glsl"),
                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/Volume/fragment.glsl")
                    << ShaderInfo::FragmentShaderFile(":shaders/Helpers/fragment.glsl")
                    ),

                           const ShaderInfo::ShaderVariablesNames & attributeArrays =
                ShaderInfo::ShaderVariablesNames() << "vertex" << "tex",

                           const ShaderInfo::ShaderVariablesNames & uniformValues =
                ShaderInfo::ShaderVariablesNames() << "view" << "model" << "projection" <<
                "scale" << "eye" << "modelBillboard" << "lightView" << "slope" << "intercept" << "windowCenter"
                             << "windowWidth" << "huRange" << "valueRange");

        void init(const TextureInfo::Size & size, const VolumeInfo::PhysicalSize & physicalSize, const TextureInfo::Scaling & scaling);

        virtual void rotate(const QVector3D & rotation, const qreal & speed);

        virtual Camera::Matrix mvp(const Viewport::Viewport * viewport) const;
        virtual Camera::Matrix childsMVP(const Viewport::Viewport * viewport, const AbstractModel * child) const;

        virtual Camera::ViewMatrix lightView(const Viewport::Viewport * viewport) const;

        virtual Camera::ModelMatrix model(const Viewport::Viewport * viewport) const;

        VolumeInfo::Slope slope() const;
        VolumeInfo::Intercept intercept() const;

        VolumeInfo::WindowCenter windowCenter() const;
        VolumeInfo::WindowWidth windowWidth() const;

        VolumeInfo::HuRange huRange() const;
        VolumeInfo::ValueRange valueRange() const;

    protected:
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;
        
        virtual void drawingRoutine() const;

    private:
        VolumeInfo::Slope _slope;
        VolumeInfo::Intercept _intercept;

        VolumeInfo::WindowCenter _windowCenter;
        VolumeInfo::WindowWidth _windowWidth;

        VolumeInfo::HuRange _huRange;
        VolumeInfo::ValueRange _valueRange;

    public slots:
        void setSlope(const VolumeInfo::Slope & slope);
        void setIntercept(const VolumeInfo::Intercept & intercept);

        void setWindowCenter(const VolumeInfo::WindowCenter & windowCenter);
        void setWindowWidth(const VolumeInfo::WindowWidth & windowWidth);

        void setHuRange(const VolumeInfo::HuRange & huRange);
        void setValueRange(const VolumeInfo::ValueRange & valueRange);
    };
}
#endif // VOLUMEMODEL_H
