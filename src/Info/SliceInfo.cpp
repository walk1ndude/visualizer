#include "Info/SliceInfo.h"

namespace SliceInfo {
    Slices::Slices() { }

    Slices::Slices(TextureInfo::Texture texture,
                   const HuRange & huRange,
                   const PhysicalSize & physicalSize) :
        texture(texture),
        huRange(huRange),
        physicalSize(physicalSize) {
    }
}
