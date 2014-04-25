#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include "viewport.h"

class ViewPorts {
public:
    explicit ViewPorts(const QVector<QPair<QRectF, ViewPort::ProjectionType> > & viewPorts =
            QVector<QPair<QRectF, ViewPort::ProjectionType> >(), const QSize & windowSize = QSize());
    ~ViewPorts();

    ViewPort & operator [](int i);

    void setViewPorts(const QVector<QPair<QRectF, ViewPort::ProjectionType> > & viewPorts, const QSize & windowSize);

    void scale(const QVector3D & scale);
    void rotate(qreal xRot, qreal yRot, qreal zRot);
    void zoom(const qreal & factor);

    void resize(const QSize & windowSize);

private:
    QVector<ViewPort>_viewPorts;
};

#endif // VIEWPORTS_H
