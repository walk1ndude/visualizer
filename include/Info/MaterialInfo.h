#ifndef MATERIALINFO_H
#define MATERIALINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace MaterialInfo {
    using Emissive = QVector4D;
    using Diffuse = QVector4D;
    using Specular = QVector4D;
    using Shininess = GLfloat;

    using MaterialID = QString;

    using Materials = QHash<MaterialID, ShaderInfo::ShaderVariablesNames>;

    class Material : public QQuickItem {
        Q_PROPERTY(QVector4D emissive READ emissive WRITE setEmissive NOTIFY emissiveChanged)
        Q_PROPERTY(QVector4D diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
        Q_PROPERTY(QVector4D specular READ specular WRITE setSpecular NOTIFY specularChanged)
        Q_PROPERTY(qreal shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

        Q_PROPERTY(QString name READ id WRITE setID NOTIFY idChanged)

        Q_OBJECT
    public:
        Material();
        Material(const MaterialID & id,
                 const Emissive & emissive,
                 const Diffuse & diffuse,
                 const Specular & specular,
                 const Shininess & shininess);

        Emissive emissive() const;
        Diffuse diffuse() const;
        Specular specular() const;
        Shininess shininess() const;

        MaterialID id() const;

        static QStringList initializationOrder;

    private:
        Emissive _emissive;
        Diffuse _diffuse;
        Specular _specular;
        Shininess _shininess;

        MaterialID _id;

    signals:
        void emissiveChanged(const Emissive & emissive);
        void diffuseChanged(const Diffuse & diffuse);
        void specularChanged(const Specular & specular);
        void shininessChanged(const Shininess & shininess);
        void idChanged(const MaterialID & id);

    public slots:
        void setEmissive(const Emissive & emissive);
        void setDiffuse(const Diffuse & diffuse);
        void setSpecular(const Specular & specular);
        void setShininess(const Shininess & shininess);
        void setID(const MaterialID & id);
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

#endif // MATERIALINFO_H
