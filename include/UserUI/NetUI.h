#ifndef NETUI_H
#define NETUI_H

#include <QtNetwork/QTcpServer>

#include "Message/SettingsMessage.h"

namespace UserUI {
    class NetUI : public QObject {
        Q_OBJECT
    public:
        explicit NetUI(const QString & host = "127.0.0.1", const qint16 & port = 5555, QObject * parent = nullptr);

    private:
        QTcpServer * _dataServer;

        void fetchConnections();

    public slots:
        virtual void recieve(const Message::SettingsMessage & message);
    };
}

#endif // NETUI_H
