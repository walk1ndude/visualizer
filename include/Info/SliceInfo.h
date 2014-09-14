#ifndef SLICEINFO_H
#define SLICEINFO_H

#include "Info/Info.h"
#include "Info/TextureInfo.h"
#include "Info/ModelInfo.h"

namespace SliceInfo {
    using HuRange = QVector2D;
    using PhysicalSize = QVector3D;

    class Slices {
    public:
        TextureInfo::Texture texture;
        HuRange huRange;
        PhysicalSize physicalSize;

        Slices();
        Slices(TextureInfo::Texture texture,
               const HuRange & huRange,
               const PhysicalSize & physicalSize);
    };
}

Q_DECLARE_METATYPE(SliceInfo::Slices)

#endif // SLICEINFO_H
