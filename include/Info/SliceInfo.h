#ifndef SLICEINFO_H
#define SLICEINFO_H

#include "Info/Info.h"
#include "Info/TextureInfo.h"

namespace SliceInfo {
    using HuRange = QVector2D;

    class Slices {
    public:
        TextureInfo::Texture texture;
        HuRange huRange;

        Slices() { }

        Slices(TextureInfo::Texture texture,
               const HuRange & huRange) :
            texture(texture),
            huRange(huRange) {
        }
    };
}

Q_DECLARE_METATYPE(SliceInfo::Slices)

#endif // SLICEINFO_H
