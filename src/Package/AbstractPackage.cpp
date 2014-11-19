#include <QtCore/QDateTime>

#include "Package/AbstractPackage.h"

namespace Package {
    Header::Header(const Sender & sender, const Reciever & reciever,
                   const ReliableTime & reliableTime,
                   QObject * ) :
        _sender(sender),
        _reciever(reciever),
        _reliableTime(reliableTime) {
        _timeStamp = QDateTime::currentMSecsSinceEpoch();
    }

    AbstractPackage::AbstractPackage(const Sender & sender, const Reciever & reciever,
                                     const ReliableTime & reliableTime,
                                     QObject * ) {
        _header = new Header(sender, reciever, reliableTime, this);
    }

    const Header * AbstractPackage::header() const {
        return _header;
    }

    bool AbstractPackage::isMessageReliable() const {
        return _header->_reliableTime <= QDateTime::currentMSecsSinceEpoch() - _header->_timeStamp;
    }

    Sender AbstractPackage::sender() const {
        return _header->_sender;
    }

    Reciever AbstractPackage::reciever() const {
        return _header->_reciever;
    }
}
