#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"

#include "ViewPort/ViewPortArray.h"

#include "Info/SliceInfo.h"
#include "Info/TextureInfo.h"

#include "Scene/ModelScene.h"

namespace Render {
    class ModelRenderer : public AbstractRenderer {
        Q_OBJECT
    public:
        explicit ModelRenderer(QOpenGLContext * context, const QSize & size);
        ~ModelRenderer();

        void selectScene(Scene::AbstractScene * scene = nullptr);

    protected:
        virtual void initialize();
        virtual void updateTextures();
        virtual void render();

    private:
        Scene::AbstractScene * _selectedScene;

        // remember all scenes, rendered by this renderer -> for clean up after
        QSet<Scene::AbstractScene *> _sceneHistory;

        SliceInfo::SliceSettings _sliceSettings;

        void cleanUp();

        void updateTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                               const QOpenGLTexture::TextureFormat & textureFormat,
                               const QOpenGLTexture::PixelFormat & pixelFormat,
                               const QOpenGLTexture::PixelType & pixelType,
                               const QOpenGLPixelTransferOptions * pixelOptions);


    public slots:
        void drawSlices(SliceInfo::SliceSettings sliceSettings);

        void addStlModel(ModelInfo::BuffersVN buffers);

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
    };
}
#endif // SLICERENDERER_H
