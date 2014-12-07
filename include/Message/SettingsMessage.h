#ifndef SETTINGSPACKAGE_H
#define SETTINGSPACKAGE_H

#include "Message/AbstractMessage.h"

namespace Message {
    using SettingsData = QVariantMap;

    class SettingsMessage : public AbstractMessage {
    public:
        explicit SettingsMessage();
        explicit SettingsMessage(const Sender & sender, const Reciever & reciever,
                                 const ReliableTime & reliableTime = -1);

        SettingsData data;
   };
}

Q_DECLARE_METATYPE(Message::SettingsMessage)

#endif // SETTINGSPACKAGE_H
