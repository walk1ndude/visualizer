#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "Render/AbstractRenderer.h"
#include "Render/ViewPortArray.h"

#include "Info/SliceInfo.h"
#include "Info/TextureInfo.h"

namespace Render {
    class ModelRenderer : public AbstractRenderer {
        Q_OBJECT
    public:
        explicit ModelRenderer(QOpenGLContext * context, const QSize & size);
        ~ModelRenderer();

        void addTexture(TextureInfo::Texture & textureInfo);

        // some constants here
        void initMaterials();
        void initLightSources();

        void addMaterial(const MaterialInfo::Emissive & emissive,
                         const MaterialInfo::Diffuse & diffuse,
                         const MaterialInfo::Specular & specular,
                         const MaterialInfo::Shininess & shininess);

        void addLightSource(const LightInfo::Position & position,
                            const LightInfo::Color & color,
                            const LightInfo::AmbientIntensity & ambientIntensity);

    protected:
        QHash<QOpenGLTexture *, TextureInfo::Texture> _textures;

        QMultiHash<Model::AbstractModel *, QOpenGLTexture *> _models;
        QMultiHash<Model::AbstractModel *, MaterialInfo::Material *> _materialsInModel;

        QVector<MaterialInfo::Material *> _materials;
        QVector<LightInfo::LightSource *> _lightSources;

        Model::AbstractModel * _selectedModel;
        QOpenGLTexture * _selectedTexture;

        virtual void initialize();
        virtual void updateTextures();
        virtual void render();

    private:
        ViewPortArray _viewPorts;

        qreal _zoomFactor;

        SliceInfo::SliceSettings _sliceSettings;

        void initializeViewPorts();
        void cleanUp();

        void bindTextures(Model::AbstractModel * model);
        void releaseTextures();

        QList<QOpenGLTexture *> _bindedTextures;

        void updateTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                               const QOpenGLTexture::TextureFormat & textureFormat,
                               const QOpenGLTexture::PixelFormat & pixelFormat,
                               const QOpenGLTexture::PixelType & pixelType,
                               const QOpenGLPixelTransferOptions * pixelOptions);


    public slots:
        void drawSlices(SliceInfo::SliceSettings sliceSettings);

        void addStlModel(ModelInfo::BuffersVN &buffers);

        // to take shots of the rotated model
        void setTakeShot(const bool & takeShot);

        // rotate selected model
        void setRotation(const QVector3D & rotation);

        // zoom the whole scene
        void setZoomFactor(const qreal & zoomFactor);

        // to clip selected model
        void setSRange(const ModelInfo::TexelAxisRange & sRange);
        void setTRange(const ModelInfo::TexelAxisRange & tRange);
        void setPRange(const ModelInfo::TexelAxisRange &pRange);
    };
}
#endif // SLICERENDERER_H
