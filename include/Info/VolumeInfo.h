#ifndef VOLUMEINFO_H
#define VOLUMEINFO_H

#include "Info/Info.h"
#include "Info/TextureInfo.h"
#include "Info/ModelInfo.h"

namespace VolumeInfo {
    using PhysicalSize = QVector3D;

    class Volume {
    public:
        TextureInfo::Texture texture;
        PhysicalSize physicalSize;

        Volume();
        Volume(TextureInfo::Texture texture,
               const PhysicalSize & physicalSize);
    };
}

Q_DECLARE_METATYPE(VolumeInfo::Volume)

#endif // VOLUMEINFO_H
