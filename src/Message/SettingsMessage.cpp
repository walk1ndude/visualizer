#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage(QObject * parent) :
        AbstractMessage(parent) {

    }

    SettingsMessage::SettingsMessage(const SettingsMessage & message) :
        AbstractMessage(message) {
        data = message.data;
    }

    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime, QObject * parent) :
        AbstractMessage(sender, reciever, reliableTime, parent) {

    }
}
