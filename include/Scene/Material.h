#ifndef MATERIAL_H
#define MATERIAL_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"
#include "Info/MaterialInfo.h"

#include "Scene/SceneObject.h"

namespace Scene {
    using Materials = QHash<ObjectID, ShaderInfo::ShaderVariablesNames>;

    class Material : public SceneObject {
        Q_PROPERTY(QVector4D emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
        Q_PROPERTY(QVector4D diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
        Q_PROPERTY(QVector4D specular READ specular WRITE setSpecular NOTIFY specularChanged)
        Q_PROPERTY(qreal shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

        Q_OBJECT
    public:
        explicit Material();
        explicit Material(const ObjectID & id,
                          const MaterialInfo::Emissive & emissive,
                          const MaterialInfo::Diffuse & diffuse,
                          const MaterialInfo::Specular & specular,
                          const MaterialInfo::Shininess & shininess);
        explicit Material(const MaterialInfo::Params & params);

        MaterialInfo::Emissive emissive() const;
        MaterialInfo::Diffuse diffuse() const;
        MaterialInfo::Specular specular() const;
        MaterialInfo::Shininess shininess() const;

        static QStringList initializationOrder;

    private:
        MaterialInfo::Emissive _emissive;
        MaterialInfo::Diffuse _diffuse;
        MaterialInfo::Specular _specular;
        MaterialInfo::Shininess _shininess;

    signals:
        void emissiveChanged(const MaterialInfo::Emissive & emissive);
        void diffuseChanged(const MaterialInfo::Diffuse & diffuse);
        void specularChanged(const MaterialInfo::Specular & specular);
        void shininessChanged(const MaterialInfo::Shininess & shininess);

    public slots:
        void setEmissive(const MaterialInfo::Emissive & emissive);
        void setDiffuse(const MaterialInfo::Diffuse & diffuse);
        void setSpecular(const MaterialInfo::Specular & specular);
        void setShininess(const MaterialInfo::Shininess & shininess);
    };

    class MaterialProgram {
    public:
        MaterialProgram(QOpenGLShaderProgram * program,
                        const ShaderInfo::ShaderVariablesNames & shaderVariables);

        void setUniform(QOpenGLShaderProgram * program,
                        Material * material);

    private:
        ShaderInfo::ShaderVariable shaderEmissive;
        ShaderInfo::ShaderVariable shaderDiffuse;
        ShaderInfo::ShaderVariable shaderSpecular;
        ShaderInfo::ShaderVariable shaderShininess;

    };
}

#endif // MATERIAL_H
