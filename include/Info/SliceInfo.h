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
               const HuRange & huRange) {
            this->texture = texture;
            this->huRange = huRange;
        }
    };
}

#endif // SLICEINFO_H
