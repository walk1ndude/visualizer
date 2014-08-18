#ifndef MODELSCENE_H
#define MODELSCENE_H

#include "Scene/AbstractScene.h"

#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/SliceInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

#include "Model/StlModel.h"

#include "Viewport/ViewportArray.h"

namespace Scene {
    class ModelScene : public AbstractScene {
        Q_OBJECT
    public:
        explicit ModelScene();
        explicit ModelScene(Viewport::ViewportArray ** viewPortArray);
        ~ModelScene();

        virtual void renderScene(const QSize & surfaceSize);

        virtual void cleanUp();

        QVector3D rotation();

        QRect screenSaveRect();

        void addTexture(TextureInfo::Texture & textureInfo);

        virtual void setRotation(const QVector3D & rotation);

        // to clip selected model
        virtual void setXRange(const ModelInfo::ViewAxisRange & xRange);
        virtual void setYRange(const ModelInfo::ViewAxisRange & yRange);
        virtual void setZRange(const ModelInfo::ViewAxisRange & zRange);

        virtual void addPoint(const PointsInfo::Point & point);

    private:
        QMap<QOpenGLTexture *, TextureInfo::Texture> _texturesInfo;
        QList<Model::AbstractModel *> _models;

        QMultiMap<Model::AbstractModel *, QOpenGLTexture *> _texturesInModel;
        QMultiMap<Model::AbstractModel *, MaterialInfo::Material *> _materialsInModel;

        QList<MaterialInfo::Material *> _materials;
        QList<LightInfo::LightSource *> _lightSources;
        QList<QOpenGLTexture *> _textures;

        Model::AbstractModel * _selectedModel;
        QOpenGLTexture * _selectedTexture;

        QVector3D _rotation;

        Viewport::ViewportRect _screenSaveRect;

        void selectModel(Model::AbstractModel * model);

    signals:
        void modelIDChanged(const uint & modelID);

    public slots:
        void addStlModel(ModelInfo::BuffersVN buffers);
        void addHeadModel(SliceInfo::Slices slices);
    };

}

#endif // MODELSCENE_H
