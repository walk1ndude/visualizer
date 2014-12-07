#ifndef MATERIALINFO_H
#define MATERIALINFO_H

#include "Info/Info.h"

namespace MaterialInfo {
    using Emissive = QVector4D;
    using Diffuse = QVector4D;
    using Specular = QVector4D;
    using Shininess = GLfloat;

    using Params = QVariantMap;
}

#endif // MATERIALINFO_H
