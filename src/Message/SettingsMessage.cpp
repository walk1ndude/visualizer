#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage() :
        AbstractMessage() {

    }

    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime) :
        AbstractMessage(sender, reciever, reliableTime) {

    }
}
