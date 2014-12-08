#ifndef VIEWRANGEINFO_H
#define VIEWRANGEINFO_H

#include "Info/ShaderInfo.h"

#define DEFAULT_VIEWAXISRANGE ViewRangeInfo::ViewAxisRange(-1.0f, 1.0f)

namespace ViewRangeInfo {
    using ViewAxisRange = QVector2D;

    enum ViewAxis {
        XAXIS,
        YAXIS,
        ZAXIS
    };

    using ViewAxisRanges = QVector<ViewAxisRange>;

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

Q_DECLARE_METATYPE(ViewRangeInfo::ViewAxisRange)
Q_DECLARE_METATYPE(ViewRangeInfo::ViewAxis)

#endif // VIEWRANGEINFO_H
