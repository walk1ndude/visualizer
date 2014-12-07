#ifndef TEXTUREINFO_H
#define TEXTUREINFO_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

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
}

Q_DECLARE_METATYPE(TextureInfo::TextureInfo)

#endif // TEXTUREINFO_H
