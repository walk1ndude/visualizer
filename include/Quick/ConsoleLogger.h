#ifndef CONSOLEVIEWER_H
#define CONSOLEVIEWER_H

#include <QtQuick/QQuickItem>

namespace Quick {
    class ConsoleLogger : public QQuickItem {
        Q_PROPERTY(QString output READ output WRITE setOutput NOTIFY outputChanged)
        Q_PROPERTY(int lineCount READ lineCount WRITE setLineCount NOTIFY lineCountChanged)

        Q_OBJECT
    public:
        explicit ConsoleLogger();

        static void customMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);

        QString output();
        void setOutput(const QString & output);

        int lineCount();
        void setLineCount(const int & lineCount);

    private:
        static QString _output;

        int _lineCount;

    signals:
        void outputChanged();
        void lineCountChanged();
    };
}

#endif // CONSOLEVIEWER_H
