#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"

#include "Scene/ModelScene.h"

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

    public slots:
        virtual void addModel(const ModelInfo::Model & model);

        virtual void recieve(const Message::SettingsMessage & message);
    };
}
#endif // SLICERENDERER_H
