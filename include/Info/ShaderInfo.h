#ifndef SHADERINFO_H
#define SHADERINFO_H

#include "Info/Info.h"

namespace ShaderInfo {
    using VertexShaderFile = QString;
    using FragmentShaderFile = QString;
    using GeometryShaderFile = QString;
    using TesselationControlShaderFile = QString;
    using TesselationEvaluationShaderFile = QString;

    using VertexShaderFiles = QList<VertexShaderFile>;
    using FragmentShaderFiles = QList<FragmentShaderFile>;
    using GeometryShaderFiles = QList<GeometryShaderFile>;
    using TesselationControlShaderFiles = QList<TesselationControlShaderFile>;
    using TesselationEvaluationShaderFiles = QList<TesselationEvaluationShaderFile>;

    using ShaderVariable = int;

    using ShaderVariableName = QString;
    using ShaderVariablesNames = QList<ShaderVariableName>;

    class ShaderFiles {
    public:
        VertexShaderFiles vertexShaderFiles;
        FragmentShaderFiles fragmentShaderFiles;
        GeometryShaderFiles geometryShaderFiles;
        TesselationControlShaderFiles tesselationControlShaderFiles;
        TesselationEvaluationShaderFiles tesselationEvaluationShaderFiles;

        ShaderFiles() { }

        ShaderFiles(const VertexShaderFiles & vertexShaderFiles,
                    const FragmentShaderFiles & fragmentShaderFiles,
                    const GeometryShaderFiles & geometryShaderFiles = QList<GeometryShaderFile>(),
                    const TesselationControlShaderFiles & tesselationControlShaderFiles = QList<TesselationControlShaderFile>(),
                    const TesselationEvaluationShaderFiles & tesselationEvaluationShaderFiles = QList<TesselationEvaluationShaderFile>()) {
            this->vertexShaderFiles = vertexShaderFiles;
            this->fragmentShaderFiles = fragmentShaderFiles;
            this->geometryShaderFiles = geometryShaderFiles;
            this->tesselationControlShaderFiles = tesselationControlShaderFiles;
            this->tesselationEvaluationShaderFiles = tesselationEvaluationShaderFiles;
        }
    };
}

#endif // SHADERINFO_H
