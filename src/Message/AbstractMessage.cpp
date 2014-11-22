#include <QtCore/QDateTime>

#include "Message/AbstractMessage.h"

namespace Message {
    Header::Header(const Sender & sender, const Reciever & reciever,
                   const ReliableTime & reliableTime,
                   QObject * ) :
        _sender(sender),
        _reciever(reciever),
        _reliableTime(reliableTime) {
        _timeStamp = QDateTime::currentMSecsSinceEpoch();
    }

    AbstractMessage::AbstractMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime,
                                     QObject * ) {
        _header = new Header(sender, reciever, reliableTime, this);
    }

    const Header * AbstractMessage::header() const {
        return _header;
    }

    bool AbstractMessage::isMessageReliable() const {
        return _header->_reliableTime <= QDateTime::currentMSecsSinceEpoch() - _header->_timeStamp;
    }

    Sender AbstractMessage::sender() const {
        return _header->_sender;
    }

    Reciever AbstractMessage::reciever() const {
        return _header->_reciever;
    }
}
