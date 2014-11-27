#ifndef VIEWRANGEINFO_H
#define VIEWRANGEINFO_H

#include "Info/ShaderInfo.h"

namespace ViewRangeInfo {
    class ViewAxisRange : public QVector2D {
    public:
        ViewAxisRange(const qreal & minV = -1.0f, const qreal & maxV = 1.0f) :
            QVector2D(minV, maxV) {
        }
    };

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
