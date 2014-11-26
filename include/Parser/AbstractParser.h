#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include <QtCore/QVariant>
#include <QtCore/QUrl>

#include <QtCore/QDebug>

#include "Message/SettingsMessage.h"

namespace Parser {
    class AbstractParser : public QObject {
        Q_PROPERTY(QUrl file READ file WRITE setFile NOTIFY fileChanged)
        Q_PROPERTY(QVariant files READ files WRITE setFiles NOTIFY filesChanged)

        Q_PROPERTY(QVariant blueprint READ blueprint WRITE setBlueprint NOTIFY blueprintChanged)

        Q_OBJECT
    public:
        QUrl file() const;
        QVariant files() const;

        QVariant blueprint() const;

    protected:
        explicit AbstractParser();

        template<class T>
        void sendResults(T result, QVariantMap & map) {
            map["data"] = QVariant::fromValue<T>(result);

            emit finished(map);
        }

        QVariant _blueprint;

    signals:
        void finished(QVariantMap model);

        void send(const Message::SettingsMessage & model);

        void fileChanged();
        void filesChanged();

        void blueprintChanged(const QVariant & blueprint);

    public slots:
        virtual void setFile(const QUrl & file);
        virtual void setFiles(const QVariant & files);

        virtual void setBlueprint(const QVariant & blueprint);
    };
}

#endif // ABSTRACTPARSER_H
