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
        Q_OBJECT

        Q_PROPERTY(bool takeShot READ takeShot WRITE setTakeShot NOTIFY takeShotChanged)

        Q_PROPERTY(QPointF selectedPointPosition READ selectedPointPosition WRITE setSelectedPointPosition)
        Q_PROPERTY(QString selectedPointName READ selectedPointName WRITE setSelectedPointName)
        Q_PROPERTY(QColor selectedPointColor READ selectedPointColor WRITE setSelectedPointColor)

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

        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        Q_INVOKABLE void mouseRotation(const QPointF & prevPos, const QPointF & finishPos);

        QSize fboSize();
        void setFboSize(const QSize & fboSize);

        bool takeShot();
        void setTakeShot(const bool & takeShot);

        QPointF selectedPointPosition();
        void setSelectedPointPosition(const QPointF & position);

        QString selectedPointName();
        void setSelectedPointName(const QString & name);

        QColor selectedPointColor();
        void setSelectedPointColor(const QColor & color);

        QVector3D rotation();
        void setRotation(const QVector3D & rotation);

        qreal zoomFactor();
        void setZoomFactor(const qreal & zoomFactor);

        ModelInfo::ViewAxisRange xRange();
        void setXRange(const ModelInfo::ViewAxisRange & xRange);

        ModelInfo::ViewAxisRange yRange();
        void setYRange(const ModelInfo::ViewAxisRange & yRange);

        ModelInfo::ViewAxisRange zRange();
        void setZRange(const ModelInfo::ViewAxisRange & zRange);

        QVector2D huRange();
        void sethuRange(const QVector2D & huRange);

        int minHU();
        void setMinHU(const int & minHU);

        int maxHU();
        void setMaxHU(const int & maxHU);

        int modelID();
        void setModelID(const int & modelID);

        Viewport::ViewportArray * viewportArray();
        void setViewportArray(Viewport::ViewportArray * viewportArray);

        // for now just add empty scene
        void addModelScene();

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData * paintNodeData);

    private:
        Render::ModelRenderer * _modelRenderer;

        Viewport::ViewportArray * _viewportArray;

        QVector<Scene::AbstractScene *> _scenes;

        bool _takeShot;

        PointsInfo::Point _selectedPoint;

        QVector3D _step;

        QSize _fboSize;

        ModelInfo::ViewAxisRange _xRange;
        ModelInfo::ViewAxisRange _yRange;
        ModelInfo::ViewAxisRange _zRange;

        QVector2D _huRange;

        qreal _zoomFactor;

        int _minHU;
        int _maxHU;

        QVector3D _rotation;

        uint _modelID;

    signals:
        void appearedSmthToDraw();

        void takeShotChanged(const bool & takeShot);
        void rotationChanged(const QVector3D & rotation);
        void zoomFactorChanged(const qreal & zoomFactor);

        void xRangeChanged(const ModelInfo::ViewAxisRange & xRange);
        void yRangeChanged(const ModelInfo::ViewAxisRange & yRange);
        void zRangeChanged(const ModelInfo::ViewAxisRange & zRange);

        void mouseRotationChanged(const QPointF & startPos, const QPointF & finishPos);

        void viewportArrayChanged();

        void huRangeChanged();

        void minHUChanged(const int & minHU);
        void maxHUChanged(const int & maxHU);

        void pointAdded(const PointsInfo::Point & point);

        void slicesProcessed(SliceInfo::Slices slices);
        void modelRead(ModelInfo::BuffersVN buffers);

        void pointUpdated(const QVariantMap & point);

        void modelIDChanged(const uint & modelID);

    public slots:
        void drawSlices(SliceInfo::Slices slices);
        void updatePoint(const PointsInfo::UpdatedPoint & point);
    };
}

#endif // SLICEVIEWER_H
