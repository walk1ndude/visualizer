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

        virtual void init(const ModelInfo::Params & params);

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

    private:
        VolumeInfo::Slope _slope;
        VolumeInfo::Intercept _intercept;

        VolumeInfo::WindowCenter _windowCenter;
        VolumeInfo::WindowWidth _windowWidth;

        VolumeInfo::HuRange _huRange;
        VolumeInfo::ValueRange _valueRange;

    public slots:
        virtual void setSlope(const VolumeInfo::Slope & slope);
        virtual void setIntercept(const VolumeInfo::Intercept & intercept);

        virtual void setWindowCenter(const VolumeInfo::WindowCenter & windowCenter);
        virtual void setWindowWidth(const VolumeInfo::WindowWidth & windowWidth);

        virtual void setHuRange(const VolumeInfo::HuRange & huRange);
        virtual void setValueRange(const VolumeInfo::ValueRange & valueRange);

        virtual void invoke(const QString & name, const ModelInfo::Params & params = ModelInfo::Params());
    };
}
#endif // VOLUMEMODEL_H
