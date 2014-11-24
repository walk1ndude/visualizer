#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime, QObject * parent) :
        AbstractMessage(sender, reciever, reliableTime, parent) {

    }
}
