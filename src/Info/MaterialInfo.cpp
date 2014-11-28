#include "Info/MaterialInfo.h"

static int materialNumber = 0;

namespace MaterialInfo {
    Material::Material() :
        _id(getNewID<MaterialID>(materialNumber)) {
    }

    QStringList Material::initializationOrder = { "emissive", "diffuse", "specular", "shineness"};

    Material::Material(const MaterialID & id,
                       const Emissive & emissive,
                       const Diffuse & diffuse,
                       const Specular & specular,
                       const Shininess & shininess) :
        Material() {
        _id = id;
        _emissive = emissive;
        _diffuse = diffuse;
        _specular = specular;
        _shininess = shininess;
    }

    Material::Material(const Params & params) {
        if (params.contains("id")) {
            _id = params["id"].value<MaterialID>();
        }
        else {
            _id = getNewID<MaterialID>(materialNumber);
        }

        _emissive = params["emissive"].value<Emissive>();
        _diffuse = params["diffuse"].value<Diffuse>();
        _specular = params["specular"].value<Specular>();
        _shininess = params["shininess"].value<Shininess>();
    }

    Emissive Material::emissive() const {
        return _emissive;
    }

    Diffuse Material::diffuse() const {
        return _diffuse;
    }

    Specular Material::specular() const {
        return _specular;
    }

    Shininess Material::shininess() const {
        return _shininess;
    }

    MaterialID Material::id() const {
        return _id;
    }

    void Material::setID(const MaterialID & id) {
        _id = id;

        emit idChanged(id);
    }

    void Material::setEmissive(const Emissive & emissive) {
        _emissive = emissive;

        emit emissiveChanged(emissive);
    }

    void Material::setDiffuse(const Diffuse & diffuse) {
        _diffuse = diffuse;

        emit diffuseChanged(diffuse);
    }

    void Material::setSpecular(const Specular & specular) {
        _specular = specular;

        emit specularChanged(specular);
    }

    void Material::setShininess(const Shininess & shininess) {
        _shininess = shininess;

        emit shininessChanged(shininess);
    }

    MaterialProgram::MaterialProgram(QOpenGLShaderProgram * program,
                                     const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        shaderEmissive(program->uniformLocation(shaderVariables.at(0))),
        shaderDiffuse(program->uniformLocation(shaderVariables.at(1))),
        shaderSpecular(program->uniformLocation(shaderVariables.at(2))),
        shaderShininess(program->uniformLocation(shaderVariables.at(3))) {
    }

    void MaterialProgram::setUniform(QOpenGLShaderProgram * program,
                                     Material * material) {
        program->setUniformValue(shaderEmissive, material->emissive());
        program->setUniformValue(shaderDiffuse, material->diffuse());
        program->setUniformValue(shaderSpecular, material->specular());
        program->setUniformValue(shaderShininess, material->shininess());
}

}
