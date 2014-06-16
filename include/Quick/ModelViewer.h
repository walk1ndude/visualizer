#ifndef SLICEVIEWER_H
#define SLICEVIEWER_H

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_4_1_Core>
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

        Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
        Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

        Q_PROPERTY(QVector2D xRange READ xRange WRITE setXRange NOTIFY xRangeChanged)
        Q_PROPERTY(QVector2D yRange READ yRange WRITE setYRange NOTIFY yRangeChanged)
        Q_PROPERTY(QVector2D zRange READ zRange WRITE setZRange NOTIFY zRangeChanged)

        Q_PROPERTY(QVector2D huRange READ huRange WRITE sethuRange NOTIFY huRangeChanged)

        Q_PROPERTY(int minHU READ minHU WRITE setMinHU NOTIFY minHUChanged)
        Q_PROPERTY(int maxHU READ maxHU WRITE setMaxHU NOTIFY maxHUChanged)

    public:
        explicit ModelViewer();
        virtual ~ModelViewer();

        bool takeShot();
        void setTakeShot(const bool & takeShot);

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

        // for now just add empty scene
        void addModelScene();

    protected:
        bool _needsInitialize;

        QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

    private:
        Render::ModelRenderer * _modelRenderer;

        QVector<Scene::AbstractScene *> _scenes;

        bool _takeShot;

        QVector3D _step;

        ModelInfo::ViewAxisRange _xRange;
        ModelInfo::ViewAxisRange _yRange;
        ModelInfo::ViewAxisRange _zRange;

        QVector2D _huRange;

        qreal _zoomFactor;

        int _minHU;
        int _maxHU;

        QVector3D _rotation;

    signals:
        void appearedSmthToDraw();

        void takeShotChanged(const bool & takeShot);
        void rotationChanged(const QVector3D & rotation);
        void zoomFactorChanged(const qreal & zoomFactor);

        void xRangeChanged(const ModelInfo::ViewAxisRange & xRange);
        void yRangeChanged(const ModelInfo::ViewAxisRange & yRange);
        void zRangeChanged(const ModelInfo::ViewAxisRange & zRange);

        void huRangeChanged();

        void minHUChanged(const int & minHU);
        void maxHUChanged(const int & maxHU);

        void slicesProcessed(SliceInfo::Slices slices);
        void modelRead(ModelInfo::BuffersVN buffers);

    public slots:
        void drawSlices(SliceInfo::Slices slices);
    };
}

#endif // SLICEVIEWER_H
