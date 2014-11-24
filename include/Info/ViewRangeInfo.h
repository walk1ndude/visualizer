#ifndef VIEWRANGEINFO_H
#define VIEWRANGEINFO_H

#include "Info/ShaderInfo.h"

namespace ViewRangeInfo {
    using ViewAxisRange = QVector2D;

    enum ViewAxis {
        XAXIS,
        YAXIS,
        ZAXIS
    };

    class ViewRange {
    public:
        ViewAxisRange xRange;
        ViewAxisRange yRange;
        ViewAxisRange zRange;

        ViewRange(const ViewAxisRange & xRange,
                  const ViewAxisRange & yRange,
                  const ViewAxisRange & zRange,
                  QOpenGLShaderProgram * program,
                  const ShaderInfo::ShaderVariablesNames & shaderVariables);

        void setUniformValue(QOpenGLShaderProgram * program);

        void setViewAxisRange(const ViewAxisRange & viewAxisRange,
                               const ViewAxis viewAxis);

    private:
        ShaderInfo::ShaderVariable shaderXRange;
        ShaderInfo::ShaderVariable shaderYRange;
        ShaderInfo::ShaderVariable shaderZRange;
    };
}

Q_DECLARE_METATYPE(ViewRangeInfo::ViewAxis)

#endif // VIEWRANGEINFO_H
