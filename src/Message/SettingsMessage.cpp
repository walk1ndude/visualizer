#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage() :
        AbstractMessage() {

    }

    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime) :
        AbstractMessage(sender, reciever, reliableTime) {

    }

    QVariantMap SettingsMessage::toVariantMap(const SettingsMessage & message) {
        QVariantMap map;

        map["header"] = QVariantMap() = {
            { "sender", message.sender() },
            { "reciever", message.reciever() },
            { "reliableTime", message.reliableTime() }
        };

        QVariantMap mapData;

        for (const QString & key : message.data.keys()) {
            mapData[key] = message.data[key];
        }

        map["data"] = mapData;

        return map;
    }

    SettingsMessage SettingsMessage::toMessage(const QVariant &message) {
        QVariantMap messageMap = message.toMap();

        QVariantMap msHeader = messageMap["header"].toMap();

        Message::SettingsMessage ms(
                    msHeader["sender"].value<Message::Sender>(),
                    msHeader["reciever"].value<Message::Reciever>()
                );

        if (msHeader.contains("reliableTime")) {
            ms.setReliableTime(msHeader["reliableTime"].value<Message::ReliableTime>());
        }

        ms.data = messageMap["data"].toMap();

        return ms;
    }
 }
