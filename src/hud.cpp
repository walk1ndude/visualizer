#include <QtQml/QQmlContext>

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
    QObject * root = (QObject *) this->rootObject();

    QObject::connect(root, SIGNAL(rBottomChanged(qreal)), this, SIGNAL(rBottomChanged(qreal)));
    QObject::connect(root, SIGNAL(rTopChanged(qreal)), this, SIGNAL(rTopChanged(qreal)));

    QObject::connect(root, SIGNAL(xRotChanged(qreal)), this, SIGNAL(xRotChanged(qreal)));
    QObject::connect(root, SIGNAL(yRotChanged(qreal)), this, SIGNAL(yRotChanged(qreal)));
    QObject::connect(root, SIGNAL(zRotChanged(qreal)), this, SIGNAL(zRotChanged(qreal)));
    QObject::connect(root, SIGNAL(distChanged(qreal)), this, SIGNAL(distChanged(qreal)));
}
