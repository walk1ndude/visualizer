#ifndef NETUI_H
#define NETUI_H

#include <QtNetwork/QTcpServer>

#include <QtCore/QJsonObject>

#include "Message/SettingsMessage.h"

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 5555

namespace UserUI {
    class NetUI : public QObject {
        Q_OBJECT
    public:
        explicit NetUI(const QString & host = DEFAULT_HOST, const qint16 & port = DEFAULT_PORT, QObject * parent = nullptr);

    private:
        QTcpServer * _dataServer;

        QByteArray _dataToSend;

        void fetchConnections();

    public slots:
        virtual void recieve(const Message::SettingsMessage & message);

    private slots:
        void sendData();
    };
}

#endif // NETUI_H
