#include "Info/LightInfo.h"

static uint lightSourceNumber = 0;

namespace LightInfo {
    LightSource::LightSource() :
        SceneInfo::SceneObject(SceneInfo::getNewID(lightSourceNumber), 0) {

    }

    QStringList LightSource::initializationOrder = {"position", "color", "ambientIntensity", "attenuation"};

    LightSource::LightSource(const SceneInfo::ObjectID & lightID,
                             const Position & position,
                             const Color & color,
                             const AmbientIntensity & ambientIntensity,
                             const Attenuation & attenuation) :
        SceneInfo::SceneObject(lightID, &lightSourceNumber) {
        _position = position;
        _color = color;
        _ambientIntensity = ambientIntensity;
        _attenuation = attenuation;
    }

    LightSource::LightSource(const Params & params) :
        SceneInfo::SceneObject(params["id"].value<SceneInfo::ObjectID>(), &lightSourceNumber) {

        _position = params["position"].value<Position>();
        _color = params["color"].value<Color>();
        _ambientIntensity = params["ambientIntensity"].value<AmbientIntensity>();
        _attenuation = params["attenuation"].value<Attenuation>();
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

    Attenuation LightSource::attenuation() const {
        return _attenuation;
    }

    void LightSource::setPosition(const Position & position) {
        _position = position;

        emit positionChanged(position);
    }

    void LightSource::setColor(const Color & color) {
        _color = color;

        emit colorChanged(color);
    }

    void LightSource::setAmbientIntensity(const AmbientIntensity & ambientIntensity) {
        _ambientIntensity = ambientIntensity;

        emit ambientIntensityChanged(ambientIntensity);
    }

    void LightSource::setAttenuation(const Attenuation & attenuation) {
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
