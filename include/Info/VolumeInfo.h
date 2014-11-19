#ifndef VOLUMEINFO_H
#define VOLUMEINFO_H

#include "Info/Info.h"
#include "Info/TextureInfo.h"
#include "Info/ModelInfo.h"

namespace VolumeInfo {
    using PhysicalSize = QVector3D;

    using Slope = float;
    using Intercept = float;

    using WindowCenter = int;
    using WindowWidth = int;

    using ValueRange = QVector2D;
    using HuRange = QVector2D;

    class Volume {
    public:
        TextureInfo::Texture texture;
        PhysicalSize physicalSize;

        Slope slope;
        Intercept intercept;

        WindowCenter windowCenter;
        WindowWidth windowWidth;

        ValueRange valueRange;
        HuRange huRange;

        Volume();
        Volume(TextureInfo::Texture texture,
               const PhysicalSize & physicalSize,
               const Slope & slope,
               const Intercept & intercept,
               const WindowCenter & windowCenter,
               const WindowWidth & windowWidth);
    };
}

Q_DECLARE_METATYPE(VolumeInfo::Volume)

#endif // VOLUMEINFO_H
