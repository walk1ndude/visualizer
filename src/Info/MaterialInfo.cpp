#include "Info/MaterialInfo.h"

static MaterialInfo::MaterialID materialNumber = 0;

namespace MaterialInfo {
    Material::Material() :
        _id(materialNumber ++) {
    }

    Material::Material(const Emissive & emissive,
                       const Diffuse & diffuse,
                       const Specular & specular,
                       const Shininess & shininess) :
        Material() {
        _emissive = emissive;
        _diffuse = diffuse;
        _specular = specular;
        _shininess = shininess;
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

    uint Material::id() const {
        return _id;
    }

    void Material::setEmissive(const Emissive & emissive) {
        _emissive = emissive;

        emit emissiveChanged();
    }

    void Material::setDiffuse(const Diffuse & diffuse) {
        _diffuse = diffuse;

        emit diffuseChanged();
    }

    void Material::setSpecular(const Specular & specular) {
        _specular = specular;

        emit specularChanged();
    }

    void Material::setShininess(const Shininess & shininess) {
        _shininess = shininess;

        emit shininessChanged();
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
