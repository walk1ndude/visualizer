#ifndef MATERIALINFO_H
#define MATERIALINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

static uint materialNumber = 0;

namespace MaterialInfo {
    using Emissive = QVector4D;
    using Diffuse = QVector4D;
    using Specular = QVector4D;
    using Shininess = GLfloat;

    class Material : public QQuickItem {
        Q_PROPERTY(QVector4D emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
        Q_PROPERTY(QVector4D diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
        Q_PROPERTY(QVector4D specular READ specular WRITE setSpecular NOTIFY specularChanged)
        Q_PROPERTY(qreal shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

        Q_OBJECT
    public:
        Material() { }
        Material(const Emissive & emissive,
                 const Diffuse & diffuse,
                 const Specular & specular,
                 const Shininess & shininess) :
            _emissive(emissive),
            _diffuse(diffuse),
            _specular(specular),
            _shininess(shininess),
            _id(materialNumber ++) {
        }

        Emissive emissive() const {
            return _emissive;
        }

        Diffuse diffuse() const {
            return _diffuse;
        }

        Specular specular() const {
            return _specular;
        }

        Shininess shininess() const {
            return _shininess;
        }

        uint id() { return _id; }
    private:
        Emissive _emissive;
        Diffuse _diffuse;
        Specular _specular;
        Shininess _shininess;

        uint _id;

    signals:
        void emissiveChanged();
        void diffuseChanged();
        void specularChanged();
        void shininessChanged();

    public slots:
        void setEmissive(const Emissive & emissive) {
            _emissive = emissive;

            emit emissiveChanged();
        }

        void setDiffuse(const Diffuse & diffuse) {
            _diffuse = diffuse;

            emit diffuseChanged();
        }

        void setSpecular(const Specular & specular) {
            _specular = specular;

            emit specularChanged();
        }

        void setShininess(const Shininess & shininess) {
            _shininess = shininess;

            emit shininessChanged();
        }
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
            program->setUniformValue(shaderEmissive, material->emissive());
            program->setUniformValue(shaderDiffuse, material->diffuse());
            program->setUniformValue(shaderSpecular, material->specular());
            program->setUniformValue(shaderShininess, material->shininess());
        }

    private:
        ShaderInfo::ShaderVariable shaderEmissive;
        ShaderInfo::ShaderVariable shaderDiffuse;
        ShaderInfo::ShaderVariable shaderSpecular;
        ShaderInfo::ShaderVariable shaderShininess;

    };
}

#endif // MATERIALINFO_H
