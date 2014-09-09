#ifndef TEXTUREINFO_H
#define TEXTUREINFO_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Info/ShaderInfo.h"

namespace TextureInfo {
    using MergedData = quint8;
    using MergedDataPtr = MergedData *;
    using MergedDataPointer = QSharedPointer<MergedData>;

    using Scaling = QVector3D;
    using Size = QVector3D;

    class Texture {
    public:
        MergedDataPointer mergedData;

        Scaling scaling;
        Size size;

        QOpenGLPixelTransferOptions pixelTransferOptions;

        QOpenGLTexture::PixelType pixelType;
        QOpenGLTexture::Target target;
        QOpenGLTexture::TextureFormat textureFormat;
        QOpenGLTexture::PixelFormat pixelFormat;
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

#endif // TEXTUREINFO_H
