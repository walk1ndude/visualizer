#ifndef LIGHTINFO_H
#define LIGHTINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace LightInfo {
    using Position = QVector4D;
    using Color = QVector4D;

    using AmbientIntensity = GLfloat;

    class LightSource : public QQuickItem {
        Q_PROPERTY(QVector4D position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QVector4D color READ color WRITE setColor NOTIFY colorChanged)
        Q_PROPERTY(qreal ambientIntensity READ ambientIntensity WRITE setAmbientIntensity NOTIFY ambientIntensityChanged)
        Q_OBJECT

    public:
        LightSource();
        LightSource(const Position & position,
                    const Color & color,
                    const AmbientIntensity & ambientIntensity);

        Position position() const;
        Color color() const;
        AmbientIntensity ambientIntensity() const;

        uint id() const;
    private:
        Position _position;
        Color _color;
        AmbientIntensity _ambientIntensity;

        uint _id;

    signals:
        void positionChanged();
        void colorChanged();
        void ambientIntensityChanged();

    public slots:
        void setPosition(const Position & position);
        void setColor(const Color & color);
        void setAmbientIntensity(const AmbientIntensity & ambientIntensity);
    };

    class LightProgram {
    public:
        LightProgram(QOpenGLShaderProgram * program,
                     const ShaderInfo::ShaderVariablesNames & shaderVariables);

        void setUniformValue(QOpenGLShaderProgram * program,
                             LightSource * lightSource);

    private:
        ShaderInfo::ShaderVariable shaderPosition;
        ShaderInfo::ShaderVariable shaderColor;
        ShaderInfo::ShaderVariable shaderAmbientIntensity;
    };
}

#endif // LIGHTINFO_H
