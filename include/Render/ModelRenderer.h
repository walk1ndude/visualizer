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
        virtual void addStlModel(ModelInfo::BuffersVN buffers) final;
        virtual void addHeadModel(SliceInfo::Slices slices) final;

        virtual void addEvaluatorModel(const int & width, const int & height = 10,
                                       const qreal & stepX = 10.0f, const qreal & stepY = 0.0f,
                                       const QVector3D & color = QVector3D(0.0f, 0.0f, 0.5f)) final;

        virtual void addEvaluatorModel(const QSize & size = QSize(10, 10),
                                       const qreal & stepX = 10.0f, const qreal & stepY = 0.0f,
                                       const QVector3D & color = QVector3D(0.0f, 0.0f, 0.5f)) final;

        // rotate selected model
        virtual void setRotation(const QVector3D & rotation);

        // to clip selected model
        virtual void setXRange(const ViewRangeInfo::ViewAxisRange & xRange);
        virtual void setYRange(const ViewRangeInfo::ViewAxisRange & yRange);
        virtual void setZRange(const ViewRangeInfo::ViewAxisRange & zRange);

        virtual void addPoint(const PointsInfo::Point & point) final;
        virtual void hidePoint(const QString & point) final;
    };
}
#endif // SLICERENDERER_H
