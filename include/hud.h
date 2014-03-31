#ifndef HUD_H
#define HUD_H

#include <QtQuick/QQuickView>

class Hud : public QQuickView {
    Q_OBJECT
public:
    explicit Hud(QWindow * parent = 0);
    ~Hud();

private:
    void fetchConnections();

signals:
    void rBottomChanged(qreal rBottom);
    void rTopChanged(qreal rTop);

    void xRotChanged(qreal xRot);
    void yRotChanged(qreal yRot);
    void zRotChanged(qreal zRot);
    void distChanged(qreal dist);
};

#endif // HUD_H
