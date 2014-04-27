#include "viewports.h"

ViewPorts::ViewPorts(const QVector<QPair<QRectF, ViewPort::ProjectionType> > & viewPorts, const QSize & windowSize) {
    if (viewPorts.size()) {
        setViewPorts(viewPorts, windowSize);
    }
}

ViewPorts::~ViewPorts() {

}

void ViewPorts::setViewPorts(const QVector<QPair<QRectF, ViewPort::ProjectionType> > & viewPorts, const QSize & windowSize) {
    _viewPorts.clear();

    QVectorIterator<QPair<QRectF, ViewPort::ProjectionType> >it(viewPorts);
    QPair<QRectF, ViewPort::ProjectionType> viewPort;

    while (it.hasNext()) {
        viewPort = it.next();
        _viewPorts.push_back(ViewPort(viewPort.first, windowSize, viewPort.second));
    }
}

void ViewPorts::resize(const QSize & windowSize) {
    for (int i = 0; i != _viewPorts.size(); ++ i) {
        _viewPorts[i].resize(windowSize);
    }
}

ViewPort & ViewPorts::operator [](int i) {
    return _viewPorts[i];
}

void ViewPorts::scale(const QVector3D & scale) {
    for (int i = 0; i != _viewPorts.size(); ++ i) {
        _viewPorts[i].scale(scale);
    }
}

void ViewPorts::rotate(qreal xRot, qreal yRot, qreal zRot) {
    for (int i = 0; i != _viewPorts.size(); ++ i) {

        switch (_viewPorts[i].projectionType()) {
            case ViewPort::PERSPECTIVE:
                _viewPorts[i].rotate(QVector3D(xRot - 90.0, yRot, zRot));
                break;
            case ViewPort::BOTTOM:
                _viewPorts[i].rotate(QVector3D(0.0, 0.0, zRot));
                break;
            case ViewPort::FRONT:
                _viewPorts[i].rotate(QVector3D(xRot - 90.0, 0.0, 0.0));
                break;
            case ViewPort::LEFT:
                _viewPorts[i].rotate(QVector3D(-90.0, yRot - 90.0, 0.0));
                break;
        }
    }
}

void ViewPorts::zoom(const qreal & zoomFactor) {
    for (int i = 0; i != _viewPorts.size(); ++ i) {
        _viewPorts[i].zoom(zoomFactor);
    }
}
