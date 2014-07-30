#ifndef MATERIALINFO_H
#define MATERIALINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

static uint materialNumber = 0;

namespace MaterialInfo {
    using Emissive = QVector4D;
    using Diffuse = QVector4D;
    using Specular = QVector4D;
    using Shininess = float;

    class Material {
    public:
        Emissive emissive;
        Diffuse diffuse;
        Specular specular;
        Shininess shininess;

        Material(const Emissive & emissive,
                 const Diffuse & diffuse,
                 const Specular & specular,
                 const Shininess & shininess) :
            emissive(emissive),
            diffuse(diffuse),
            specular(specular),
            shininess(shininess),
            _id(materialNumber ++) {
        }

        uint id() { return _id; }
    private:
        uint _id;
    };

    class MaterialProgram {
    public:
        MaterialProgram(QOpenGLShaderProgram * program,
                        const ShaderInfo::ShaderVariablesNames & shaderVariables) :
            shaderEmissive(program->uniformLocation(shaderVariables.at(0))),
            shaderDiffuse(program->uniformLocation(shaderVariables.at(1))),
            shaderSpecular(program->uniformLocation(shaderVariables.at(2))),
            shaderShininess(program->uniformLocation(shaderVariables.at(3))) {
        }

        void setUniform(QOpenGLShaderProgram * program,
                        Material * material) {
            program->setUniformValue(shaderEmissive, material->emissive);
            program->setUniformValue(shaderDiffuse, material->diffuse);
            program->setUniformValue(shaderSpecular, material->specular);
            program->setUniformValue(shaderShininess, material->shininess);
        }

    private:
        ShaderInfo::ShaderVariable shaderEmissive;
        ShaderInfo::ShaderVariable shaderDiffuse;
        ShaderInfo::ShaderVariable shaderSpecular;
        ShaderInfo::ShaderVariable shaderShininess;

    };
}

#endif // MATERIALINFO_H
