#ifndef PACKAGE_H
#define PACKAGE_H

#include "Info/Info.h"

namespace Message {
    using TimeStamp = quint64;
    using ReliableTime = quint64;

    using Sender = QString;
    using Reciever = QString;

    class Header : protected QObject {
        friend class AbstractMessage;
    public:
        explicit Header(const Sender & sender, const Reciever & reciever,
                        const ReliableTime & reliableTime = 100000,
                        QObject * parent = nullptr);

    private:
        Sender _sender;
        Reciever _reciever;

        TimeStamp _timeStamp;
        ReliableTime _reliableTime;
    };

    class AbstractMessage : protected QObject {
    public:
        virtual const Header * header() const final;

        virtual bool isMessageReliable() const;

        Sender sender() const;
        Reciever reciever() const;

    protected:
        explicit AbstractMessage(const Sender & sender, const Reciever & reciever,
                                 const ReliableTime & reliableTime = 100000,
                                 QObject * parent = nullptr);

    private:
        Header * _header;
    };
}

#endif // PACKAGE_H
