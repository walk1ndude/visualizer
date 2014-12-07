#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include "Viewport/ViewportArray.h"

#include "Info/ViewRangeInfo.h"
#include "Info/PointsInfo.h"
#include "Info/VolumeInfo.h"
#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/TextureInfo.h"

#include "Scene/Material.h"
#include "Scene/LightSource.h"
#include "Scene/Texture.h"
#include "Scene/SceneObjectList.h"

#include "Message/SettingsMessage.h"

namespace Scene {
    class AbstractScene : public QQuickItem {
        Q_PROPERTY(qreal scalingFactor READ scalingFactor WRITE setScalingFactor NOTIFY scalingFactorChanged)
        Q_PROPERTY(MeasureUnits measureUnits READ measureUnits WRITE setMeasureUnits NOTIFY measureUnitsChanged)

        Q_ENUMS(MeasureUnits)

        Q_OBJECT
    public:
        enum MeasureUnits {
            MM,
            M,
            FT
        };

        AbstractScene();

        ~AbstractScene();

        MeasureUnits measureUnits() const;

        virtual void renderScene(const QSize & surfaceSize) = 0;
        virtual void cleanUp();

        virtual QVector3D rotation() const = 0;

        bool isInitialized() const;

        qreal scalingFactor() const;

        Material * material(const ObjectID & id = ObjectID()) const;
        LightSource * lightSource(const ObjectID & id = ObjectID()) const;
        Texture * texture(const ObjectID & id = ObjectID()) const;

    protected:
        SceneObjectList<Material *> materials;
        SceneObjectList<LightSource *> lightSources;
        SceneObjectList<Texture *> textures;

        virtual void initScene() = 0;

    private:
        // to convert to gl coordinates, meaning 1 gl =
        qreal _scalingFactor;

        bool _isInitialized;

        MeasureUnits _mUnits;

    signals:
        void redraw();

        void scalingFactorChanged();
        void measureUnitsChanged();

        void post(const Message::SettingsMessage & message);

    public slots:
        virtual void initializeScene() final;

        virtual void setScalingFactor(const qreal & scalingFactor) final;
        virtual void setMeasureUnits(const MeasureUnits & mUnits) final;

        virtual void updateScene() = 0;

        virtual void recieve(const Message::SettingsMessage & message) = 0;
    };
}

#endif // ABSTRACTSCENE_H
