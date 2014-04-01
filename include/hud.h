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

    void angleChanged(qreal xRot, qreal yRot, qreal zRot);
    void zoomZChanged(qreal zoomZ);
};

#endif // HUD_H
