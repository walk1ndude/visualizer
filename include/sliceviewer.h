#ifndef OPENGLITEM_H
#define OPENGLITEM_H

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#include <QtGui/QOpenGLFunctions_4_1_Core>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QVector3D>
#include <QtGui/QImage>

#include "slicerenderer.h"

class SliceViewer : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(bool takeShot READ takeShot WRITE setTakeShot NOTIFY takeShotChanged)

    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

    Q_PROPERTY(QVector2D sRange READ sRange WRITE setSRange NOTIFY sRangeChanged)
    Q_PROPERTY(QVector2D tRange READ tRange WRITE setTRange NOTIFY tRangeChanged)
    Q_PROPERTY(QVector2D pRange READ pRange WRITE setPRange NOTIFY pRangeChanged)

    Q_PROPERTY(QVector2D huRange READ huRange WRITE sethuRange NOTIFY huRangeChanged)

    Q_PROPERTY(int minHU READ minHU WRITE setMinHU NOTIFY minHUChanged)
    Q_PROPERTY(int maxHU READ maxHU WRITE setMaxHU NOTIFY maxHUChanged)

public:
    SliceViewer();
    virtual ~SliceViewer();

    bool takeShot();
    void setTakeShot(const bool & takeShot);

    QVector3D rotation();
    void setRotation(const QVector3D & rotation);

    qreal zoomFactor();
    void setZoomFactor(const qreal & zoomFactor);

    QVector2D sRange();
    void setSRange(const QVector2D & sRange);

    QVector2D tRange();
    void setTRange(const QVector2D & tRange);

    QVector2D pRange();
    void setPRange(const QVector2D & pRange);

    QVector2D huRange();
    void sethuRange(const QVector2D & huRange);

    int minHU();
    void setMinHU(const int & minHU);

    int maxHU();
    void setMaxHU(const int & maxHU);

protected:
    bool _needsInitialize;

    QSGNode * updatePaintNode(QSGNode * node, UpdatePaintNodeData *);

private:
    SliceRenderer * _sliceRenderer;

    bool _takeShot;

    QVector3D _step;

    QVector2D _sRange;
    QVector2D _tRange;
    QVector2D _pRange;

    QVector2D _huRange;

    qreal _zoomFactor;

    int64_t _minHU;
    int64_t _maxHU;

    QVector3D _rotation;

signals:
    void initialized();

    void takeShotChanged(const bool & takeShot);
    void rotationChanged(const QVector3D & rotation);
    void zoomFactorChanged(const qreal & zoomFactor);

    void sRangeChanged(const QVector2D & sRange);
    void tRangeChanged(const QVector2D & tRange);
    void pRangeChanged(const QVector2D & pRange);

    void huRangeChanged();

    void minHUChanged(const int & minHU);
    void maxHUChanged(const int & maxHU);

    void slicesProcessed(QSharedPointer<uchar> mergedData,
                         const std::vector<float> & scaling, const std::vector<size_t> & size,
                         const int & alignment, const size_t & rowLength);

public slots:
    void drawSlices(QSharedPointer<uchar> mergedData,
                    const std::vector<float> & scaling = std::vector<float>(),
                    const std::vector<size_t> & size = std::vector<size_t>(),
                    const int & alignment = 0, const size_t & rowLength = 0,
                    const std::vector<int> & huRange = std::vector<int>());
};

#endif // OPENGLITEM_H
