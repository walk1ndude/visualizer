#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include <QtCore/QVariant>
#include <QtCore/QUrl>

namespace Parser {
    class AbstractParser : public QObject {
        Q_PROPERTY(QUrl file READ file WRITE setFile NOTIFY fileChanged)
        Q_PROPERTY(QVariant files READ files WRITE setFiles NOTIFY filesChanged)

        Q_OBJECT
    public:
        QUrl file() const;
        QVariant files() const;

    protected:
        explicit AbstractParser();

        template<class T>
        void sendResults(T result, QVariantMap & map) {
            map["data"] = QVariant::fromValue<T>(result);

            emit finished(map);
        }

    signals:
        void finished(QVariantMap model);

        void fileChanged();
        void filesChanged();

    public slots:
        virtual void setFile(const QUrl & file);
        virtual void setFiles(const QVariant & files);
    };
}

#endif // ABSTRACTPARSER_H
