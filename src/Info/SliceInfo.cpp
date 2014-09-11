#include "Info/SliceInfo.h"

namespace SliceInfo {
    Slices::Slices() { }

    Slices::Slices(TextureInfo::Texture texture,
                   const HuRange & huRange,
                   const ModelInfo::ImageSpacings & imageSpacings) :
        texture(texture),
        huRange(huRange),
        imageSpacings(imageSpacings) {
    }
}
