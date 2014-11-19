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

        Q_PROPERTY(QSize fboSize READ fboSize WRITE setFboSize NOTIFY fboSizeChanged)

        Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)

        Q_PROPERTY(QVector2D xRange READ xRange WRITE setXRange NOTIFY xRangeChanged)
        Q_PROPERTY(QVector2D yRange READ yRange WRITE setYRange NOTIFY yRangeChanged)
        Q_PROPERTY(QVector2D zRange READ zRange WRITE setZRange NOTIFY zRangeChanged)

        Q_PROPERTY(int modelID READ modelID WRITE setModelID NOTIFY modelIDChanged)

        Q_PROPERTY(QVector2D huRange READ huRange WRITE setHuRange NOTIFY huRangeChanged)

        Q_PROPERTY(Scene::ModelScene * modelScene READ modelScene WRITE setModelScene NOTIFY modelSceneChanged)

        Q_PROPERTY(Viewport::ViewportArray * viewportArray READ viewportArray WRITE setViewportArray NOTIFY viewportArrayChanged)

        Q_OBJECT
    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        QSize fboSize() const;
        
        QVariantMap selectedPoint() const;

        ViewRangeInfo::ViewAxisRange xRange() const;
        ViewRangeInfo::ViewAxisRange yRange() const;
        ViewRangeInfo::ViewAxisRange zRange() const;

        QVector3D rotation() const;

        int modelID() const;

        Viewport::ViewportArray * viewportArray() const;

        Scene::ModelScene * modelScene() const;

        VolumeInfo::HuRange huRange() const;

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData * paintNodeData);

    private:
        Render::ModelRenderer * _modelRenderer;

        Viewport::ViewportArray * _viewportArray;

        QVector<Scene::ModelScene *> _modelScenes;

        QVariantMap _selectedPoint;

        QSize _fboSize;

        QVector3D _rotation;

        ViewRangeInfo::ViewAxisRange _xRange;
        ViewRangeInfo::ViewAxisRange _yRange;
        ViewRangeInfo::ViewAxisRange _zRange;

        uint _modelID;

    signals:
        void xRangeChanged(const ViewRangeInfo::ViewAxisRange & xRange);
        void yRangeChanged(const ViewRangeInfo::ViewAxisRange & yRange);
        void zRangeChanged(const ViewRangeInfo::ViewAxisRange & zRange);

        void viewportArrayChanged();

        void rotationChanged(const QVector3D & rotation);
        void pointAdded(const PointsInfo::Point & point);

        Q_INVOKABLE void drawModel(ModelInfo::BuffersVN model);
        Q_INVOKABLE void drawModel(VolumeInfo::Volume model);

        void pointUpdated(const QVariantMap & point);

        void modelIDChanged(const uint & modelID);

        void modelSceneChanged();

        void huRangeChanged(const VolumeInfo::HuRange & huRange);

        void togglePointChanged(const QString & point);

        void fboSizeChanged(const QSize & fboSize);

    public slots:
        Q_INVOKABLE virtual void addPoint(const QPointF & point, Viewport::Viewport * viewport);
        Q_INVOKABLE virtual void togglePoint(const QString & point);

        virtual void setModelScene(Scene::ModelScene * modelScene);

        virtual void updatePoint(const PointsInfo::UpdatedPoint & point);

        virtual void setFboSize(const QSize & fboSize);

        virtual void setSelectedPoint(const QVariantMap & selectedPoint);

        virtual void setRotation(const QVector3D & rotation);

        virtual void setXRange(const ViewRangeInfo::ViewAxisRange & xRange);
        virtual void setYRange(const ViewRangeInfo::ViewAxisRange & yRange);
        virtual void setZRange(const ViewRangeInfo::ViewAxisRange & zRange);

        virtual void setHuRange(const VolumeInfo::HuRange & huRange);

        virtual void setModelID(const int & modelID);
        virtual void setViewportArray(Viewport::ViewportArray * viewportArray);
    };
}

#endif // SLICEVIEWER_H
