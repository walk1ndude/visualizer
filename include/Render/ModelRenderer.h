#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"

#include "ViewPort/ViewPortArray.h"

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

        // to take shots of the rotated model
        void setTakeShot(const bool & takeShot);

        // rotate selected model
        void setRotation(const QVector3D & rotation);

        // zoom the whole scene
        void setZoomFactor(const qreal & zoomFactor);

        // to clip selected model
        void setXRange(const ModelInfo::ViewAxisRange & xRange);
        void setYRange(const ModelInfo::ViewAxisRange & yRange);
        void setZRange(const ModelInfo::ViewAxisRange & zRange);

        void addPoint(const PointsInfo::Point & point);
    };
}
#endif // SLICERENDERER_H
