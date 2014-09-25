#include "Info/VolumeInfo.h"

namespace VolumeInfo {
    Volume::Volume() { }

    Volume::Volume(TextureInfo::Texture texture,
                   const PhysicalSize & physicalSize) :
        texture(texture),
        physicalSize(physicalSize) {
    }
}
