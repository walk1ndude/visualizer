#ifndef PACKAGE_H
#define PACKAGE_H

#include "Info/Info.h"

namespace Message {
    using TimeStamp = quint64;
    using ReliableTime = qint64;

    using Sender = QString;
    using Reciever = QString;

    class Header {
        friend class AbstractMessage;
    public:
        explicit Header();
        explicit Header(const Sender & sender, const Reciever & reciever,
                        const ReliableTime & reliableTime = -1);

    private:
        Sender _sender;
        Reciever _reciever;

        TimeStamp _timeStamp;
        ReliableTime _reliableTime;
    };

    class AbstractMessage {
    public:
        virtual ~AbstractMessage();

        virtual const Header header() const final;

        virtual bool isReliable() const;

        Sender sender() const;
        Reciever reciever() const;

        void setReliableTime(const ReliableTime & time);

    protected:
        explicit AbstractMessage();
        explicit AbstractMessage(const Sender & sender, const Reciever & reciever,
                                 const ReliableTime & reliableTime = -1);

    private:
        Header _header;
   };
}

#endif // PACKAGE_H
