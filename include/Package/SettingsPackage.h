#ifndef SETTINGSPACKAGE_H
#define SETTINGSPACKAGE_H

#include "Package/AbstractPackage.h"

namespace Package {
    using SettingsData = QVariantMap;

    using SettingName = QString;
    using SettingData = QVariant;

    class SettingsPackage : public AbstractPackage {
    public:
        SettingsPackage(const Sender & sender, const Reciever & reciever,
                        const ReliableTime & reliableTime = 100000,
                        QObject * parent = nullptr);

        void insert(const SettingName & settingName, const SettingData & settingData);
        SettingData value(const SettingName & settingName) const;

    private:
        SettingsData _data;
    };
}

#endif // SETTINGSPACKAGE_H
