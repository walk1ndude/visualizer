#include "Info/ShaderInfo.h"

namespace ShaderInfo {
    ShaderFiles::ShaderFiles() { }

    ShaderFiles::ShaderFiles(const VertexShaderFiles & vertexShaderFiles,
                             const FragmentShaderFiles & fragmentShaderFiles,
                             const GeometryShaderFiles & geometryShaderFiles,
                             const TesselationControlShaderFiles & tesselationControlShaderFiles,
                             const TesselationEvaluationShaderFiles & tesselationEvaluationShaderFiles) :
        vertexShaderFiles(vertexShaderFiles),
        fragmentShaderFiles(fragmentShaderFiles),
        geometryShaderFiles(geometryShaderFiles),
        tesselationControlShaderFiles(tesselationControlShaderFiles),
        tesselationEvaluationShaderFiles(tesselationEvaluationShaderFiles) {
    }
}
