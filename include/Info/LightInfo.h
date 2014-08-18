#ifndef LIGHTINFO_H
#define LIGHTINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

static uint lightSourceNumber = 0;

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
        LightSource() { }
        LightSource(const Position & position,
                    const Color & color,
                    const AmbientIntensity & ambientIntensity) :
            _position(position),
            _color(color),
            _ambientIntensity(ambientIntensity),
            _id(lightSourceNumber ++) {
        }

        Position position() const {
            return _position;
        }

        Color color() const {
            return _color;
        }

        AmbientIntensity ambientIntensity() const {
            return _ambientIntensity;
        }

        uint id() { return _id; }
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
        void setPosition(const Position & position) {
            _position = position;

            emit positionChanged();
        }

        void setColor(const Color & color) {
            _color = color;

            emit colorChanged();
        }

        void setAmbientIntensity(const AmbientIntensity & ambientIntensity) {
            _ambientIntensity = ambientIntensity;

            emit ambientIntensityChanged();
        }
    };

    class LightProgram {
    public:
        LightProgram(QOpenGLShaderProgram * program,
                     const ShaderInfo::ShaderVariablesNames & shaderVariables) :
            shaderPosition(program->uniformLocation(shaderVariables.at(0))),
            shaderColor(program->uniformLocation(shaderVariables.at(1))),
            shaderAmbientIntensity(program->uniformLocation(shaderVariables.at(2))) {
        }

        void setUniformValue(QOpenGLShaderProgram * program,
                             LightSource * lightSource) {
            program->setUniformValue(shaderPosition, lightSource->position());
            program->setUniformValue(shaderColor, lightSource->color());
            program->setUniformValue(shaderAmbientIntensity, lightSource->ambientIntensity());
        }

    private:
        ShaderInfo::ShaderVariable shaderPosition;
        ShaderInfo::ShaderVariable shaderColor;
        ShaderInfo::ShaderVariable shaderAmbientIntensity;
    };
}

#endif // LIGHTINFO_H
