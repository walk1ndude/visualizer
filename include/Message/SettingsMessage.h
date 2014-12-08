#ifndef SETTINGSPACKAGE_H
#define SETTINGSPACKAGE_H

#include "Message/AbstractMessage.h"

namespace Message {
    using SettingsData = QVariantMap;

    class SettingsMessage : public AbstractMessage {
    public:
        explicit SettingsMessage();
        explicit SettingsMessage(const Sender & sender, const Reciever & reciever,
                                 const Recievers & recievers = Recievers(),
                                 const ReliableTime & reliableTime = ReliableTime(-1));

        SettingsData data;

        static QVariantMap toVariantMap(const SettingsMessage & message,
                                        const Sender & sender = Sender(),
                                        const Reciever & reciever = Reciever(),
                                        const Recievers & recievers = Recievers(),
                                        const ReliableTime & reliableTime = ReliableTime(-1));
        static SettingsMessage toMessage(const QVariant & message,
                                         const Sender & sender = Sender(),
                                         const Reciever & reciever = Reciever(),
                                         const Recievers & recievers = Recievers(),
                                         const ReliableTime & reliableTime = ReliableTime(-1));
   };
}

Q_DECLARE_METATYPE(Message::SettingsMessage)

#endif // SETTINGSPACKAGE_H
