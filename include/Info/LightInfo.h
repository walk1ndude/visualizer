#ifndef LIGHTINFO_H
#define LIGHTINFO_H

#include "Info/Info.h"

namespace LightInfo {
    using Position = QVector4D;
    using Color = QVector4D;

    using AmbientIntensity = GLfloat;
    using Attenuation = GLfloat;

    using Params = QVariantMap;
}

#endif // LIGHTINFO_H
