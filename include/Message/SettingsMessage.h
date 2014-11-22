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
                        const ReliableTime & reliableTime = 100000,
                        QObject * parent = nullptr);

        void insert(const SettingName & name, const SettingData & data);

        SettingData value(const SettingName & key) const;
        SettingData operator[] (const SettingName & key) const;

    private:
        SettingsData _data;
    };
}

#endif // SETTINGSPACKAGE_H
