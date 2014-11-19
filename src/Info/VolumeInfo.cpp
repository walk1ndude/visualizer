#include "Info/VolumeInfo.h"

namespace VolumeInfo {
    Volume::Volume() { }

    Volume::Volume(TextureInfo::Texture texture,
                   const PhysicalSize & physicalSize,
                   const Slope & slope,
                   const Intercept & intercept,
                   const WindowCenter & windowCenter,
                   const WindowWidth & windowWidth) :
        texture(texture),
        physicalSize(physicalSize),
        slope(slope),
        intercept(intercept),
        windowCenter(windowCenter),
        windowWidth(windowWidth) {
    }
}
