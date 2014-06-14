#ifndef MODELSCENE_H
#define MODELSCENE_H

#include "Scene/AbstractScene.h"

#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/TextureInfo.h"

#include "Model/AbstractModel.h"

#include "ViewPort/ViewPortArray.h"

namespace Scene {
    class ModelScene : public AbstractScene {
    public:
        explicit ModelScene();
        ~ModelScene();

        void initScene(const QSize & surfaceSize);
        void renderScene(const QSize & surfaceSize);

        bool isEmpty();

        void cleanUp();

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

        void addTexture(TextureInfo::Texture & textureInfo);

        void setRotation(const QVector3D & rotation);

        // zoom the whole scene
        void setZoomFactor(const qreal & zoomFactor);

        // to clip selected model
        void setXRange(const ModelInfo::ViewAxisRange & xRange);
        void setYRange(const ModelInfo::ViewAxisRange & yRange);
        void setZRange(const ModelInfo::ViewAxisRange & zRange);

    protected:
        QMap<QOpenGLTexture *, TextureInfo::Texture> _textures;
        QVector<Model::AbstractModel *> _models;

        QMultiMap<Model::AbstractModel *, QOpenGLTexture *> _texturesInModel;
        QMultiMap<Model::AbstractModel *, MaterialInfo::Material *> _materialsInModel;

        QVector<MaterialInfo::Material *> _materials;
        QVector<LightInfo::LightSource *> _lightSources;

        Model::AbstractModel * _selectedModel;
        QOpenGLTexture * _selectedTexture;

    private:
        ViewPort::ViewPortArray _viewPorts;

        QVector3D _rotation;

        void initializeViewPorts(const QSize & surfaceSize);

    public slots:
        void addStlModel(ModelInfo::BuffersVN buffers);
    };

}

#endif // MODELSCENE_H
