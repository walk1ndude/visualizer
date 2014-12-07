#include "Scene/LightSource.h"

static uint lightSourceNumber = 0;

namespace Scene {
    LightSource::LightSource() :
        SceneObject(getNewID(lightSourceNumber), 0) {

    }

    QStringList LightSource::initializationOrder = {"position", "color", "ambientIntensity", "attenuation"};

    LightSource::LightSource(const ObjectID & lightID,
                             const LightInfo::Position & position,
                             const LightInfo::Color & color,
                             const LightInfo::AmbientIntensity & ambientIntensity,
                             const LightInfo::Attenuation & attenuation) :
        SceneObject(lightID, &lightSourceNumber) {
        _position = position;
        _color = color;
        _ambientIntensity = ambientIntensity;
        _attenuation = attenuation;
    }

    LightSource::LightSource(const LightInfo::Params & params) :
        SceneObject(params["id"].value<ObjectID>(), &lightSourceNumber) {

        _position = params["position"].value<LightInfo::Position>();
        _color = params["color"].value<LightInfo::Color>();
        _ambientIntensity = params["ambientIntensity"].value<LightInfo::AmbientIntensity>();
        _attenuation = params["attenuation"].value<LightInfo::Attenuation>();
    }

    LightInfo::Position LightSource::position() const {
        return _position;
    }

    LightInfo::Color LightSource::color() const {
        return _color;
    }

    LightInfo::AmbientIntensity LightSource::ambientIntensity() const {
        return _ambientIntensity;
    }

    LightInfo::Attenuation LightSource::attenuation() const {
        return _attenuation;
    }

    void LightSource::setPosition(const LightInfo::Position & position) {
        _position = position;

        emit positionChanged(position);
    }

    void LightSource::setColor(const LightInfo::Color & color) {
        _color = color;

        emit colorChanged(color);
    }

    void LightSource::setAmbientIntensity(const LightInfo::AmbientIntensity & ambientIntensity) {
        _ambientIntensity = ambientIntensity;

        emit ambientIntensityChanged(ambientIntensity);
    }

    void LightSource::setAttenuation(const LightInfo::Attenuation & attenuation) {
        _attenuation = attenuation;

        emit attenuationChanged(attenuation);
    }

    LightProgram::LightProgram(QOpenGLShaderProgram * program,
                               const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        _shaderPosition(-1),
        _shaderColor(-1),
        _shaderAmbientIntensity(-1),
        _shaderAttenuation(-1) {

        int l = shaderVariables.length();

        if (!l) {
            return;
        }

        if (l > 0) {
            _shaderPosition = program->uniformLocation(shaderVariables.at(0));
        }

        if (l > 1) {
            _shaderColor = program->uniformLocation(shaderVariables.at(1));
        }

        if (l > 2) {
            _shaderAmbientIntensity = program->uniformLocation(shaderVariables.at(2));
        }

        if (l > 3) {
            _shaderAttenuation = program->uniformLocation(shaderVariables.at(3));
        }
    }

    void LightProgram::setUniformValue(QOpenGLShaderProgram * program,
                                       LightSource * lightSource) {
        program->setUniformValue(_shaderPosition, lightSource->position());
        program->setUniformValue(_shaderColor, lightSource->color());
        program->setUniformValue(_shaderAmbientIntensity, lightSource->ambientIntensity());
        program->setUniformValue(_shaderAttenuation, lightSource->attenuation());
    }

}

