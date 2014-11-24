#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"

#include "Viewport/ViewportArray.h"

#include "Info/VolumeInfo.h"
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
        virtual void addModel(ModelInfo::BuffersVN buffers) final;
        virtual void addModel(VolumeInfo::Volume volume) final;

        virtual void addModel(const Model::Type & type,
                              const Model::Params & initParams = Model::Params(),
                              const Model::RequestedChildren & children = Model::RequestedChildren()
                );

        virtual void recieve(const Message::SettingsMessage & message);

        virtual void setHuRange(const VolumeInfo::HuRange & huRange);

        virtual void addPoint(const PointsInfo::Point & point) final;
        virtual void hidePoint(const QString & point) final;
    };
}
#endif // SLICERENDERER_H
