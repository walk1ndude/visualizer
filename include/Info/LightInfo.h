#ifndef LIGHTINFO_H
#define LIGHTINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace LightInfo {
    using Position = QVector4D;
    using Color = QVector4D;

    using AmbientIntensity = GLfloat;
    using Attenuation = GLfloat;

    using LightID = QString;

    using LightSources = QHash<LightID, ShaderInfo::ShaderVariablesNames>;

    class LightSource : public QQuickItem {
        Q_PROPERTY(QVector4D position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QVector4D color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(qreal ambientIntensity READ ambientIntensity WRITE setAmbientIntensity NOTIFY ambientIntensityChanged)
        Q_PROPERTY(qreal attenuation READ attenuation WRITE setAttenuation NOTIFY attenuationChanged)

        Q_PROPERTY(QString name READ id WRITE setID NOTIFY idChanged)
        Q_OBJECT

    public:
        LightSource();
        LightSource(const LightID & id,
                    const Position & position,
                    const Color & color,
                    const AmbientIntensity & ambientIntensity,
                    const Attenuation & attenuation);

        Position position() const;
        Color color() const;
        AmbientIntensity ambientIntensity() const;
        Attenuation attenuation() const;

        LightID id() const;

        static QStringList initializationOrder;

    private:
        Position _position;
        Color _color;

        AmbientIntensity _ambientIntensity;
        Attenuation _attenuation;

        QString _id;

    signals:
        void positionChanged(const Position & position);
        void colorChanged(const Color & color);
        void ambientIntensityChanged(const AmbientIntensity & ambientIntensity);
        void attenuationChanged(const Attenuation & attenuation);
        void idChanged(const LightID & id);

    public slots:
        void setPosition(const Position & position);
        void setColor(const Color & color);
        void setAmbientIntensity(const AmbientIntensity & ambientIntensity);
        void setAttenuation(const Attenuation & attenuation);

        void setID(const LightID & id);
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

#endif // LIGHTINFO_H
