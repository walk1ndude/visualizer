#ifndef PACKAGE_H
#define PACKAGE_H

#include "Info/Info.h"

namespace Message {
    using TimeStamp = quint64;
    using ReliableTime = qint64;

    using Sender = QString;
    using Reciever = QString;

    class Header : protected QObject {
        friend class AbstractMessage;
    public:
        explicit Header(const Sender & sender, const Reciever & reciever,
                        const ReliableTime & reliableTime = -1,
                        QObject * parent = nullptr);

    private:
        Sender _sender;
        Reciever _reciever;

        TimeStamp _timeStamp;
        ReliableTime _reliableTime;
    };

    class AbstractMessage : public QObject {
        Q_OBJECT
    public:
        virtual const Header * header() const final;

        virtual bool isReliable() const;

        Sender sender() const;
        Reciever reciever() const;

    protected:
        explicit AbstractMessage(QObject * parent = nullptr);
        explicit AbstractMessage(const AbstractMessage & message);
        explicit AbstractMessage(const Sender & sender, const Reciever & reciever,
                                 const ReliableTime & reliableTime = -1,
                                 QObject * parent = nullptr);

    private:
        Header * _header;
    };
}

#endif // PACKAGE_H
