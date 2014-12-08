#include "Message/SettingsMessage.h"

namespace Message {
    SettingsMessage::SettingsMessage() :
        AbstractMessage() {

    }

    SettingsMessage::SettingsMessage(const Sender & sender, const Reciever & reciever,
                                     const Recievers & recievers,
                                     const ReliableTime & reliableTime) :
        AbstractMessage(sender, reciever, recievers, reliableTime) {

    }

    QVariantMap SettingsMessage::toVariantMap(const SettingsMessage & message,
                                              const Sender & sender,
                                              const Reciever & reciever,
                                              const Recievers & recievers,
                                              const ReliableTime & reliableTime) {
        QVariantMap map;

        map["header"] = QVariantMap() = {
            { "sender", sender.isEmpty() ? message.sender() : sender },
            { "reciever", reciever.isEmpty() ? message.reciever() : reciever },
            { "recievers", recievers.isEmpty() ? QVariant(message.recievers()) : QVariant(recievers) },
            { "reliableTime", reliableTime == ReliableTime(-1) ? message.reliableTime() : reliableTime }
        };

        QVariantMap mapData;

        for (const QString & key : message.data.keys()) {
            mapData[key] = message.data[key];
        }

        map["data"] = mapData;

        return map;
    }

    SettingsMessage SettingsMessage::toMessage(const QVariant & message,
                                               const Sender & sender,
                                               const Reciever & reciever,
                                               const Recievers & recievers,
                                               const ReliableTime & reliableTime) {

        QVariantMap messageMap = message.toMap();

        QVariantMap msHeader = messageMap["header"].toMap();

        Recievers recieversList;

        for (const QVariant & reciever : msHeader["recievers"].toList()) {
            recieversList << reciever.value<Reciever>();
        }

        Message::SettingsMessage ms(
                    sender.isEmpty() ? msHeader["sender"].value<Message::Sender>() : sender,
                    reciever.isEmpty() ? msHeader["reciever"].value<Message::Reciever>() : reciever,
                    recievers.isEmpty() ? recieversList : recievers
                );

        if (msHeader.contains("reliableTime")) {
            ms.setReliableTime(reliableTime == ReliableTime(-1) ? msHeader["reliableTime"].value<Message::ReliableTime>() : reliableTime);
        }

        ms.data = messageMap["data"].toMap();

        return ms;
    }
 }
