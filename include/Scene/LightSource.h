#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"
#include "Info/LightInfo.h"

#include "Scene/SceneObject.h"

namespace Scene {
    using LightSources = QHash<ObjectID, ShaderInfo::ShaderVariablesNames>;

    class LightSource : public SceneObject {
        Q_PROPERTY(QVector4D position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QVector4D color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(qreal ambientIntensity READ ambientIntensity WRITE setAmbientIntensity NOTIFY ambientIntensityChanged)
        Q_PROPERTY(qreal attenuation READ attenuation WRITE setAttenuation NOTIFY attenuationChanged)

        Q_OBJECT
    public:
        explicit LightSource();
        explicit LightSource(const ObjectID & lightId,
                             const LightInfo::Position & position,
                             const LightInfo::Color & color,
                             const LightInfo::AmbientIntensity & ambientIntensity,
                             const LightInfo::Attenuation & attenuation);
        explicit LightSource(const LightInfo::Params & params);

        LightInfo::Position position() const;
        LightInfo::Color color() const;
        LightInfo::AmbientIntensity ambientIntensity() const;
        LightInfo::Attenuation attenuation() const;

        static QStringList initializationOrder;

    private:
        LightInfo::Position _position;
        LightInfo::Color _color;

        LightInfo::AmbientIntensity _ambientIntensity;
        LightInfo::Attenuation _attenuation;

    signals:
        void positionChanged(const LightInfo::Position & position);
        void colorChanged(const LightInfo::Color & color);
        void ambientIntensityChanged(const LightInfo::AmbientIntensity & ambientIntensity);
        void attenuationChanged(const LightInfo::Attenuation & attenuation);

    public slots:
        void setPosition(const LightInfo::Position & position);
        void setColor(const LightInfo::Color & color);
        void setAmbientIntensity(const LightInfo::AmbientIntensity & ambientIntensity);
        void setAttenuation(const LightInfo::Attenuation & attenuation);
    };

    class LightProgram {
    public:
        LightProgram(QOpenGLShaderProgram * program,
                     const ShaderInfo::ShaderVariablesNames & shaderVariables);

        void setUniformValue(QOpenGLShaderProgram * program,
                             LightSource * lightSource);

    private:
        ShaderInfo::ShaderVariable _shaderPosition;
        ShaderInfo::ShaderVariable _shaderColor;
        ShaderInfo::ShaderVariable _shaderAmbientIntensity;
        ShaderInfo::ShaderVariable _shaderAttenuation;
    };
}

#endif // LIGHTSOURCE_H
