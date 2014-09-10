#ifndef SLICEVIEWER_H
#define SLICEVIEWER_H

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QVector3D>
#include <QtGui/QImage>

#include "Render/ModelRenderer.h"

namespace Quick {
    class ModelViewer : public QQuickItem {        
        Q_PROPERTY(QVariantMap selectedPoint READ selectedPoint WRITE setSelectedPoint)

        Q_PROPERTY(QSize fboSize READ fboSize WRITE setFboSize)

        Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
        Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

        Q_PROPERTY(QVector2D xRange READ xRange WRITE setXRange NOTIFY xRangeChanged)
        Q_PROPERTY(QVector2D yRange READ yRange WRITE setYRange NOTIFY yRangeChanged)
        Q_PROPERTY(QVector2D zRange READ zRange WRITE setZRange NOTIFY zRangeChanged)

        Q_PROPERTY(QVector2D huRange READ huRange WRITE sethuRange NOTIFY huRangeChanged)

        Q_PROPERTY(int minHU READ minHU WRITE setMinHU NOTIFY minHUChanged)
        Q_PROPERTY(int maxHU READ maxHU WRITE setMaxHU NOTIFY maxHUChanged)

        Q_PROPERTY(int modelID READ modelID WRITE setModelID NOTIFY modelIDChanged)

        Q_PROPERTY(Scene::ModelScene * modelScene READ modelScene WRITE setModelScene NOTIFY modelSceneChanged)

        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        QSize fboSize();
        
        QVariantMap selectedPoint();

        QVector3D rotation();

        qreal zoomFactor();

        ViewRangeInfo::ViewAxisRange xRange();
        ViewRangeInfo::ViewAxisRange yRange();
        ViewRangeInfo::ViewAxisRange zRange();

        QVector2D huRange();

        int minHU();
        int maxHU();

        int modelID();

        Viewport::ViewportArray * viewportArray();

        Scene::ModelScene * modelScene();

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData * paintNodeData);

    private:
        Render::ModelRenderer * _modelRenderer;

        Viewport::ViewportArray * _viewportArray;

        QVector<Scene::ModelScene *> _modelScenes;

        QVariantMap _selectedPoint;

        QVector3D _step;

        QSize _fboSize;

        ViewRangeInfo::ViewAxisRange _xRange;
        ViewRangeInfo::ViewAxisRange _yRange;
        ViewRangeInfo::ViewAxisRange _zRange;

        QVector2D _huRange;

        qreal _zoomFactor;

        int _minHU;
        int _maxHU;

        QVector3D _rotation;

        uint _modelID;

    signals:
        void rotationChanged(const QVector3D & rotation);
        void zoomFactorChanged(const qreal & zoomFactor);

        void xRangeChanged(const ViewRangeInfo::ViewAxisRange & xRange);
        void yRangeChanged(const ViewRangeInfo::ViewAxisRange & yRange);
        void zRangeChanged(const ViewRangeInfo::ViewAxisRange & zRange);

        void viewportArrayChanged();

        void huRangeChanged();

        void minHUChanged(const int & minHU);
        void maxHUChanged(const int & maxHU);

        void pointAdded(const PointsInfo::Point & point);

        void slicesProcessed(SliceInfo::Slices slices);

        Q_INVOKABLE void modelRead(ModelInfo::BuffersVN buffers);

        void pointUpdated(const QVariantMap & point);

        void modelIDChanged(const uint & modelID);

        void modelSceneChanged();

        void togglePointChanged(const QString & point);

    public slots:
        Q_INVOKABLE void drawSlices(SliceInfo::Slices slices);
        Q_INVOKABLE void addPoint(const QPointF & point, Viewport::Viewport * viewport);

        Q_INVOKABLE void togglePoint(const QString & point);

        void setModelScene(Scene::ModelScene * modelScene);

        void updatePoint(const PointsInfo::UpdatedPoint & point);

        void setFboSize(const QSize & fboSize);

        void setSelectedPoint(const QVariantMap & selectedPoint);

        void setRotation(const QVector3D & rotation);

        void setZoomFactor(const qreal & zoomFactor);

        void setXRange(const ViewRangeInfo::ViewAxisRange & xRange);
        void setYRange(const ViewRangeInfo::ViewAxisRange & yRange);
        void setZRange(const ViewRangeInfo::ViewAxisRange & zRange);

        void sethuRange(const QVector2D & huRange);

        void setMinHU(const int & minHU);
        void setMaxHU(const int & maxHU);

        void setModelID(const int & modelID);

        void setViewportArray(Viewport::ViewportArray * viewportArray);
    };
}

#endif // SLICEVIEWER_H
