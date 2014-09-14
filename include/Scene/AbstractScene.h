#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include "Viewport/ViewportArray.h"

#include "Info/ViewRangeInfo.h"

namespace Scene {
    class AbstractScene : public QQuickItem {
        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)
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

        virtual void renderScene(const QSize & surfaceSize) = 0;

        virtual void cleanUp() = 0;

        virtual void setRotation(const QVector3D & rotation) = 0;

        virtual QVector3D rotation() = 0;

        virtual QRect screenSaveRect() = 0;

        Viewport::ViewportArray * viewportArray() const;

        MeasureUnits measureUnits() const;

        bool isInitialized() const;

        qreal scalingFactor() const;

        qreal distance(const qreal & measure) const;

    protected:
        virtual void initScene() = 0;

    private:
        Viewport::ViewportArray * _viewportArray;

        // to convert to gl coordinates, meaning 1 gl =
        qreal _scalingFactor;

        bool _isInitialized;

        MeasureUnits _mUnits;

    signals:
        void redraw();

        void viewportArrayChanged();
        void scalingFactorChanged();

        void measureUnitsChanged();

    public slots:
        void setViewportArray(Viewport::ViewportArray * viewportArray);
        void setScalingFactor(const qreal & scalingFactor);

        // to clip selected model
        virtual void setXRange(const ViewRangeInfo::ViewAxisRange & xRange) = 0;
        virtual void setYRange(const ViewRangeInfo::ViewAxisRange & yRange) = 0;
        virtual void setZRange(const ViewRangeInfo::ViewAxisRange & zRange) = 0;

        virtual void addPoint(const PointsInfo::Point & point) = 0;
        virtual void togglePoint(const PointsInfo::Name & point) = 0;

        virtual void initializeScene() final;

        virtual void setMeasureUnits(const MeasureUnits & mUnits) final;

        virtual void updateScene() = 0;
    };
}

#endif // ABSTRACTSCENE_H
