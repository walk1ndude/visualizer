#include "UserUI/NetUI.h"

#include <QtNetwork/QHostAddress>

namespace UserUI {
    NetUI::NetUI(const QString & host, const qint16 & port, QObject * parent) :
        QObject(parent) {
        _dataServer = new QTcpServer(this);
        if (!_dataServer->listen(QHostAddress(host), port)) {

        }
    }

    void NetUI::fetchConnections() {

    }

    void NetUI::recieve(const Message::SettingsMessage & message) {

    }
}
