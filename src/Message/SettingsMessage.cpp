#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime, QObject * parent) :
        AbstractMessage(sender, reciever, reliableTime, parent) {

    }

    void SettingsMessage::insert(const SettingName & name, const SettingData & data) {
        _data.insert(name, data);
    }

    SettingData SettingsMessage::value(const SettingName & key) const {
        return (_data.find(key) != _data.end()) ? _data[key] : QVariant();
    }

    SettingData SettingsMessage::operator[] (const SettingName & key) const {
        return value(key);
    }
}
