#include <QtCore/QDateTime>

#include "Message/AbstractMessage.h"

namespace Message {
    Header::Header(const Sender & sender, const Reciever & reciever,
                   const ReliableTime & reliableTime,
                   QObject * parent) :
        QObject(parent),
        _sender(sender),
        _reciever(reciever),
        _reliableTime(reliableTime) {
        _timeStamp = QDateTime::currentMSecsSinceEpoch();
    }

    AbstractMessage::AbstractMessage(QObject * parent) :
        QObject(parent) {
        _header = new Header(Sender(""), Reciever(""));
    }

    AbstractMessage::AbstractMessage(const AbstractMessage & message) {
        _header = new Header(message.sender(), message.reciever(), message._header->_reliableTime, this);
    }

    AbstractMessage::AbstractMessage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime,
                                     QObject * parent) :
        QObject(parent) {
        _header = new Header(sender, reciever, reliableTime, this);
    }

    const Header * AbstractMessage::header() const {
        return _header;
    }

    bool AbstractMessage::isReliable() const {
        return _header->_reliableTime < 0 ? true : (quint64(_header->_reliableTime) <= QDateTime::currentMSecsSinceEpoch() - _header->_timeStamp);
    }

    Sender AbstractMessage::sender() const {
        return _header->_sender;
    }

    Reciever AbstractMessage::reciever() const {
        return _header->_reciever;
    }
}
