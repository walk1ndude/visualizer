#include "Info/ViewRangeInfo.h"

namespace ViewRangeInfo {
    ViewRange::ViewRange(const ViewAxisRange & xRange,
                         const ViewAxisRange & yRange,
                         const ViewAxisRange & zRange,
                         QOpenGLShaderProgram * program,
                         const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        xRange(xRange), yRange(yRange), zRange(zRange),
        shaderXRange(program->uniformLocation(shaderVariables.at(0))),
        shaderYRange(program->uniformLocation(shaderVariables.at(1))),
        shaderZRange(program->uniformLocation(shaderVariables.at(2))) {
    }

    void ViewRange::setUniformValue(QOpenGLShaderProgram * program) {
        program->setUniformValue(shaderXRange, xRange);
        program->setUniformValue(shaderYRange, yRange);
        program->setUniformValue(shaderZRange, zRange);
    }

    void ViewRange::setViewAxisRange(const ViewAxisRange & viewAxisRange,
                                     const ViewAxis viewAxis) {
        switch (viewAxis) {
        case XAXIS:
            xRange = viewAxisRange;
            break;
        case YAXIS:
            yRange = viewAxisRange;
            break;
        case ZAXIS:
            zRange = viewAxisRange;
            break;
        }
    }
}
