#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include "Model/AbstractModelWithPoints.h"

#include "Viewport/ViewportArray.h"

namespace Scene {
    class AbstractScene : public QQuickItem {
        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        AbstractScene();

        virtual void renderScene(const QSize & surfaceSize) = 0;

        virtual void cleanUp() = 0;

        virtual void setRotation(const QVector3D & rotation) = 0;

        virtual QVector3D rotation() = 0;

        virtual QRect screenSaveRect() = 0;

        Viewport::ViewportArray * viewportArray() const;

        bool isInitialized();

    protected:
        virtual void initScene() = 0;

    private:
        Viewport::ViewportArray * _viewportArray;

        bool _isInitialized;

    signals:
        void pointUpdated(const PointsInfo::UpdatedPoint & point);
        void redraw();

        void viewportArrayChanged();

    public slots:
        void setViewportArray(Viewport::ViewportArray * viewportArray);

        // to clip selected model
        virtual void setXRange(const ViewRangeInfo::ViewAxisRange & xRange) = 0;
        virtual void setYRange(const ViewRangeInfo::ViewAxisRange & yRange) = 0;
        virtual void setZRange(const ViewRangeInfo::ViewAxisRange & zRange) = 0;

        virtual void addPoint(const PointsInfo::Point & point) = 0;
        virtual void hidePoint(const PointsInfo::Name & point) = 0;

        virtual void initializeScene() final;

        virtual void updateScene() = 0;
    };
}

#endif // ABSTRACTSCENE_H
