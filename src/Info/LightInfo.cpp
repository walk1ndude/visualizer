#include "Info/LightInfo.h"

static uint lightSourceNumber = 0;

namespace LightInfo {
    LightSource::LightSource() { }
    LightSource::LightSource(const Position & position,
                             const Color & color,
                             const AmbientIntensity & ambientIntensity) :
        _position(position),
        _color(color),
        _ambientIntensity(ambientIntensity),
        _id(lightSourceNumber ++) {
    }

    Position LightSource::position() const {
        return _position;
    }

    Color LightSource::color() const {
        return _color;
    }

    AmbientIntensity LightSource::ambientIntensity() const {
        return _ambientIntensity;
    }

    uint LightSource::id() const {
        return _id;
    }

    void LightSource::setPosition(const Position & position) {
        _position = position;

        emit positionChanged();
    }

    void LightSource::setColor(const Color & color) {
        _color = color;

        emit colorChanged();
    }

    void LightSource::setAmbientIntensity(const AmbientIntensity & ambientIntensity) {
        _ambientIntensity = ambientIntensity;

        emit ambientIntensityChanged();
    }

    LightProgram::LightProgram(QOpenGLShaderProgram * program,
                               const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        shaderPosition(program->uniformLocation(shaderVariables.at(0))),
        shaderColor(program->uniformLocation(shaderVariables.at(1))),
        shaderAmbientIntensity(program->uniformLocation(shaderVariables.at(2))) {
    }

    void LightProgram::setUniformValue(QOpenGLShaderProgram * program,
                                       LightSource * lightSource) {
        program->setUniformValue(shaderPosition, lightSource->position());
        program->setUniformValue(shaderColor, lightSource->color());
        program->setUniformValue(shaderAmbientIntensity, lightSource->ambientIntensity());
    }

}
