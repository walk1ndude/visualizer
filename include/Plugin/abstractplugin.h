#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include "Info/Info.h"
#include "Message/SettingsMessage.h"

namespace Plugin {
    class AbstractPlugin : public QQuickItem {
        Q_OBJECT
    protected:
        explicit AbstractPlugin();

    signals:
        void post(const Message::SettingsMessage & message);
        void post(const QVariantMap & message);

    public slots:
        virtual void recieve(const Message::SettingsMessage & message) = 0;
        virtual void recieve(const QVariant & message) = 0;
    };
}

#endif // ABSTRACTPLUGIN_H

