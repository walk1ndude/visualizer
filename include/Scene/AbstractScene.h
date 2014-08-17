#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QtCore/QSize>

#include "Model/AbstractModelWithPoints.h"

#include "Viewport/ViewportArray.h"

namespace Scene {
    class AbstractScene : public QObject {
        Q_OBJECT
    public:
        AbstractScene(Viewport::ViewportArray ** viewportArray = nullptr);

        virtual void initScene() = 0;
        virtual void renderScene(const QSize & surfaceSize) = 0;

        virtual bool isEmpty() = 0;
        virtual bool isInitialized() final { return _isInitialized; }

        virtual void cleanUp() = 0;

        virtual void setRotation(const QVector3D & rotation) = 0;

        virtual QVector3D rotation() = 0;

        virtual QRect screenSaveRect() = 0;

        // to clip selected model
        virtual void setXRange(const ModelInfo::ViewAxisRange & xRange) = 0;
        virtual void setYRange(const ModelInfo::ViewAxisRange & yRange) = 0;
        virtual void setZRange(const ModelInfo::ViewAxisRange & zRange) = 0;
        
        virtual void addPoint(const PointsInfo::Point & point) = 0;

    protected:
        Viewport::ViewportArray * viewportArray();

    private:
        Viewport::ViewportArray ** _viewportArray;

        bool _isInitialized;

    signals:
        void pointUpdated(const PointsInfo::UpdatedPoint & point);
        void redraw();
    };
}

#endif // ABSTRACTSCENE_H
