#include <QtCore/QDateTime>

#include "Message/AbstractMessage.h"

namespace Message {
    Header::Header() {

    }

    Header::Header(const Sender & sender, const Reciever & reciever,
                   const ReliableTime & reliableTime) :
        _sender(sender),
        _reciever(reciever),
        _reliableTime(reliableTime) {
        _timeStamp = QDateTime::currentMSecsSinceEpoch();
    }

    AbstractMessage::AbstractMessage() {
        _header = Header(Sender(""), Reciever(""));
    }

    AbstractMessage::AbstractMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime) {
        _header = Header(sender, reciever, reliableTime);
    }

    AbstractMessage::~AbstractMessage() {

    }

    const Header AbstractMessage::header() const {
        return _header;
    }

    bool AbstractMessage::isReliable() const {
        return _header._reliableTime < 0 ? true : (quint64(_header._reliableTime) <= QDateTime::currentMSecsSinceEpoch() - _header._timeStamp);
    }

    Sender AbstractMessage::sender() const {
        return _header._sender;
    }

    Reciever AbstractMessage::reciever() const {
        return _header._reciever;
    }

    void AbstractMessage::setReliableTime(const ReliableTime & time) {
        _header._reliableTime = time;
        _header._timeStamp = QDateTime::currentMSecsSinceEpoch();
    }
}
