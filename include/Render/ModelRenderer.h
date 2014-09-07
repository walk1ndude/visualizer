#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"

#include "Viewport/ViewportArray.h"

#include "Info/SliceInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

namespace Render {
    class ModelRenderer : public AbstractRenderer {
        Q_OBJECT
    public:
        explicit ModelRenderer(QOpenGLContext * context, const QSize & size);
        ~ModelRenderer();

    protected:
        void render();

        virtual void connectWithScene(Scene::AbstractScene * scene);
        virtual void disconnectWithScene(Scene::AbstractScene * scene);

    signals:
        void modelIDChanged(const uint & modelID);
        void pointUpdated(const PointsInfo::UpdatedPoint & point);

    public slots:
        void addStlModel(ModelInfo::BuffersVN buffers);
        void addHeadModel(SliceInfo::Slices slices);

        void addEvaluatorModel(const int & width, const int & height = 100,
                               const qreal & stepX = 0.1f, const qreal & stepY = 0.0f,
                               const QVector3D & color = QVector3D(0.0f, 1.0f, 0.0f));

        void addEvaluatorModel(const QSize & size = QSize(100, 100),
                               const qreal & stepX = 0.1f, const qreal & stepY = 0.0f,
                               const QVector3D & color = QVector3D(0.0f, 1.0f, 0.0f));

        // rotate selected model
        void setRotation(const QVector3D & rotation);

        // to clip selected model
        void setXRange(const ViewRangeInfo::ViewAxisRange & xRange);
        void setYRange(const ViewRangeInfo::ViewAxisRange & yRange);
        void setZRange(const ViewRangeInfo::ViewAxisRange & zRange);

        void addPoint(const PointsInfo::Point & point);
    };
}
#endif // SLICERENDERER_H
