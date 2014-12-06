#ifndef TEXTUREINFO_H
#define TEXTUREINFO_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Info/ShaderInfo.h"
#include "Info/SceneInfo.h"

namespace TextureInfo {
    using MergedData = quint8;
    using MergedDataPtr = MergedData *;
    using MergedDataPointer = QSharedPointer<MergedData>;

    using Size = QVector3D;

    using Params = QVariantMap;

    class TextureInfo {
    public:
        MergedDataPointer mergedData;

        QOpenGLPixelTransferOptions pixelTransferOptions;

        Size size;

        QOpenGLTexture::PixelType pixelType;
        QOpenGLTexture::Target target;
        QOpenGLTexture::TextureFormat textureFormat;
        QOpenGLTexture::PixelFormat pixelFormat;
    };

    class Texture : public SceneInfo::SceneObject {
        Q_OBJECT

    public:
        explicit Texture();
        explicit Texture(QOpenGLTexture * texture,
                         const SceneInfo::ObjectID & textureID = SceneInfo::ObjectID());
        explicit Texture(const Params & params);

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

Q_DECLARE_METATYPE(TextureInfo::TextureInfo)

#endif // TEXTUREINFO_H
