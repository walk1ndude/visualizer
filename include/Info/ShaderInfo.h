#ifndef SHADERINFO_H
#define SHADERINFO_H

#include "Info/Info.h"

namespace ShaderInfo {
    using VertexShaderFile = QString;
    using FramentShaderFile = QString;
    using GeometryShaderFile = QString;
    using TesselationControlShaderFile = QString;
    using TesselationEvaluationShaderFile = QString;

    using ShaderVariable = int;

    using ShaderVariables = QStringList;

    class ShaderFiles {
    public:
        VertexShaderFile vertexShaderFile;
        FramentShaderFile framentShaderFile;
        GeometryShaderFile geometryShaderFile;
        TesselationControlShaderFile tesselationControlShaderFile;
        TesselationEvaluationShaderFile tesselationEvaluationShaderFile;

        ShaderFiles(const VertexShaderFile & vertexShaderFile,
                    const FramentShaderFile & framentShaderFile,
                    const GeometryShaderFile & geometryShaderFile = "",
                    const TesselationControlShaderFile & tesselationControlShaderFile = "",
                    const TesselationEvaluationShaderFile & tesselationEvaluationShaderFile = "") {
            this->vertexShaderFile = vertexShaderFile;
            this->framentShaderFile = framentShaderFile;
            this->geometryShaderFile = geometryShaderFile;
            this->tesselationControlShaderFile = tesselationControlShaderFile;
            this->tesselationEvaluationShaderFile = tesselationEvaluationShaderFile;
        }
    };
}

#endif // SHADERINFO_H
