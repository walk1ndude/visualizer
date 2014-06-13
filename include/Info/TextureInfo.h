#ifndef TEXTUREINFO_H
#define TEXTUREINFO_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Info/ShaderInfo.h"

namespace TextureInfo {
    using MergedData = quint8;
    using MergedDataPtr = MergedData *;
    using MergedDataPointer = QSharedPointer<MergedData>;

    using Scaling = QVector<float>;
    using Size = QVector<size_t>;

    class Texture {
    public:
        MergedDataPointer mergedData;

        Scaling scaling;
        Size size;

        QOpenGLPixelTransferOptions pixelTransferOptions;

        QOpenGLTexture::PixelType pixelType;

        bool textureDataUpdated;

        QOpenGLTexture::Target target;

        Texture(const QOpenGLTexture::Target & target,
                MergedDataPointer & mergedData,
                const Size & size,
                const Scaling & scaling,
                const QOpenGLPixelTransferOptions & pixelTransferOptions,
                const QOpenGLTexture::PixelType & pixelType) {
            this->target = target;
            this->mergedData = mergedData;

            this->scaling = scaling;
            this->size = size;

            this->pixelTransferOptions = pixelTransferOptions;

            this->pixelType = pixelType;

            this->textureDataUpdated = true;
        }
    };

    class TextureProgram {
    private:
        ShaderInfo::ShaderVariable _sampler;

    public:
        TextureProgram(QOpenGLShaderProgram * program,
                       const ShaderInfo::ShaderVariables & shaderVariables) {
            _sampler = program->uniformLocation(shaderVariables.at(0));
        }

        void setUniform(QOpenGLShaderProgram * program,
                        const uint samplerNum) {
            program->setUniformValue(_sampler, samplerNum);
        }
    };
}

#endif // TEXTUREINFO_H
