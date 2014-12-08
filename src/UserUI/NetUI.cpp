#include "UserUI/NetUI.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

namespace UserUI {
    NetUI::NetUI(const QString & host, const qint16 & port, QObject * parent) :
        QObject(parent) {
        _dataServer = new QTcpServer(this);

        fetchConnections();

        if (!_dataServer->listen(QHostAddress(host), port)) {
            qDebug() << "server hasn't started, continue without it";
        }
    }

    void NetUI::fetchConnections() {
        QObject::connect(_dataServer, &QTcpServer::newConnection, this, &NetUI::sendData);
    }

    void NetUI::recieve(const Message::SettingsMessage & message) {
        if (message.isReliable()) {
            _dataToSend.clear();

            QDataStream stream(&_dataToSend, QIODevice::ReadWrite);
            stream << message.data;
        }
    }

    void NetUI::sendData() {
        if (_dataToSend.isEmpty()) {
            return;
        }

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out << (quint16)0;
        out << _dataToSend.at(qrand() % _dataToSend.size());
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        QTcpSocket * clientConnection = _dataServer->nextPendingConnection();
        connect(clientConnection, &QTcpSocket::disconnected, clientConnection, &QTcpSocket::deleteLater);

        clientConnection->write(block);
        clientConnection->disconnectFromHost();
    }
}
