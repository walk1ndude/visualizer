#include "Scene/Material.h"

static uint materialNumber = 0;

namespace Scene {
    Material::Material() :
        SceneObject(getNewID(materialNumber)) {
    }

    QStringList Material::initializationOrder = { "emissive", "diffuse", "specular", "shineness"};

    Material::Material(const ObjectID & materialID,
                       const MaterialInfo::Emissive & emissive,
                       const MaterialInfo::Diffuse & diffuse,
                       const MaterialInfo::Specular & specular,
                       const MaterialInfo::Shininess & shininess) :
        SceneObject(materialID, &materialNumber) {
        _emissive = emissive;
        _diffuse = diffuse;
        _specular = specular;
        _shininess = shininess;
    }

    Material::Material(const MaterialInfo::Params & params) :
        SceneObject(params["id"].value<ObjectID>(), &materialNumber) {
        _emissive = params["emissive"].value<MaterialInfo::Emissive>();
        _diffuse = params["diffuse"].value<MaterialInfo::Diffuse>();
        _specular = params["specular"].value<MaterialInfo::Specular>();
        _shininess = params["shininess"].value<MaterialInfo::Shininess>();
    }

    MaterialInfo::Emissive Material::emissive() const {
        return _emissive;
    }

    MaterialInfo::Diffuse Material::diffuse() const {
        return _diffuse;
    }

    MaterialInfo::Specular Material::specular() const {
        return _specular;
    }

    MaterialInfo::Shininess Material::shininess() const {
        return _shininess;
    }

    void Material::setEmissive(const MaterialInfo::Emissive & emissive) {
        _emissive = emissive;

        emit emissiveChanged(emissive);
    }

    void Material::setDiffuse(const MaterialInfo::Diffuse & diffuse) {
        _diffuse = diffuse;

        emit diffuseChanged(diffuse);
    }

    void Material::setSpecular(const MaterialInfo::Specular & specular) {
        _specular = specular;

        emit specularChanged(specular);
    }

    void Material::setShininess(const MaterialInfo::Shininess & shininess) {
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
