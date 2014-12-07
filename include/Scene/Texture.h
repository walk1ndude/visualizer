#ifndef TEXTURE_H
#define TEXTURE_H

#include "Info/ShaderInfo.h"
#include "Info/TextureInfo.h"

#include "Scene/SceneObject.h"

namespace Scene {
    class Texture : public SceneObject {
        Q_OBJECT

    public:
        explicit Texture();
        explicit Texture(QOpenGLTexture * texture,
                         const ObjectID & textureID = ObjectID());
        explicit Texture(const TextureInfo::Params & params);

        QOpenGLTexture * texture() const;

        ~Texture();

        static QStringList initializationOrder;

    private:
        QOpenGLTexture * _texture;

    signals:
        void textureChanged(const QOpenGLTexture * texture);

    public slots:
        void setTexture(QOpenGLTexture * texture);
    };

    class TextureProgram {
    public:
        TextureProgram(QOpenGLShaderProgram * program,
                       const ShaderInfo::ShaderVariablesNames & shaderVariables);

        void setUniform(QOpenGLShaderProgram * program,
                        const uint samplerNum);

    private:
        ShaderInfo::ShaderVariable _sampler;
    };
}


#endif // TEXTURE_H
