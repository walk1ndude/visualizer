#include <QtQuick/QQuickItem>

#include <QtCore/QDebug>

#include "hud.h"

Hud::Hud(QWindow * parent) :
    QQuickView(parent) {

    setSource(QUrl("qrc:/qml/Hud.qml"));

    fetchConnections();
    setResizeMode(QQuickView::SizeRootObjectToView);
}

Hud::~Hud() {

}

void Hud::fetchConnections() {
    QQuickItem * root = this->rootObject();

    QObject * shaderGrid = root->findChild<QObject*>("shaderGrid");
    QObject * geometryGrid = root->findChild<QObject*>("geometryGrid");

    QObject::connect(shaderGrid, SIGNAL(rBottomChanged(qreal)), this, SIGNAL(rBottomChanged(qreal)));
    QObject::connect(shaderGrid, SIGNAL(rTopChanged(qreal)), this, SIGNAL(rTopChanged(qreal)));
    QObject::connect(shaderGrid, SIGNAL(ambientIntensityChanged(qreal)), this, SIGNAL(ambientIntensityChanged(qreal)));

    QObject::connect(geometryGrid, SIGNAL(angleChanged(qreal, qreal, qreal)), this, SIGNAL(angleChanged(qreal,qreal,qreal)));
    QObject::connect(geometryGrid, SIGNAL(zoomZChanged(qreal)), this, SIGNAL(zoomZChanged(qreal)));
}
