#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include "Viewport/ViewportArray.h"

#include "Info/ViewRangeInfo.h"
#include "Info/PointsInfo.h"
#include "Info/VolumeInfo.h"
#include "Info/LightInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/SceneInfo.h"

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

        MeasureUnits measureUnits() const;

        virtual void renderScene(const QSize & surfaceSize) = 0;
        virtual void cleanUp() = 0;

        virtual QVector3D rotation() const = 0;

        bool isInitialized() const;

        qreal scalingFactor() const;

        MaterialInfo::Material * material(const MaterialInfo::MaterialID & id = "") const;
        LightInfo::LightSource * lightSource(const LightInfo::LightID & id = "") const;

    protected:
        QList<MaterialInfo::Material *> materials;
        QList<LightInfo::LightSource *> lightSources;

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
