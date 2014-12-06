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

    using Scaling = QVector3D;
    using Size = QVector3D;
}

#endif // VOLUMEINFO_H
