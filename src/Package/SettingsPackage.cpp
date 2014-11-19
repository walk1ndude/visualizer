#include "Package/SettingsPackage.h"

namespace Package {
    SettingsPackage::SettingsPackage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime, QObject * parent) :
        AbstractPackage(sender, reciever, reliableTime, parent) {

    }

    void SettingsPackage::insert(const SettingName & settingName, const SettingData & settingData) {
        _data.insert(settingName, settingData);
    }

    SettingData SettingsPackage::value(const SettingName & settingName) const {
        return _data[settingName];
    }
}
