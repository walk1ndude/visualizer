#ifndef SETTINGSPACKAGE_H
#define SETTINGSPACKAGE_H

#include "Message/AbstractMessage.h"

namespace Message {
    using SettingsData = QVariantMap;

    using SettingName = QString;
    using SettingData = QVariant;

    class SettingsMessage : public AbstractMessage {
    public:
        SettingsMessage(const Sender & sender, const Reciever & reciever,
                        const ReliableTime & reliableTime = -1,
                        QObject * parent = nullptr);

        SettingsData data;
    };
}

#endif // SETTINGSPACKAGE_H
