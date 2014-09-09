#include "Info/SliceInfo.h"

namespace SliceInfo {
    Slices::Slices() { }

    Slices::Slices(TextureInfo::Texture texture,
                   const HuRange & huRange) :
        texture(texture),
        huRange(huRange) {
    }
}
