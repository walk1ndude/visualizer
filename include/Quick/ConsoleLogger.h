#ifndef CONSOLEVIEWER_H
#define CONSOLEVIEWER_H

#include <QtQuick/QQuickItem>

namespace Quick {
    class ConsoleLogger : public QQuickItem {
        Q_PROPERTY(QString output READ output WRITE setOutput NOTIFY outputChanged)
        Q_PROPERTY(QString logFile READ logFile WRITE setLogFile NOTIFY logFileChanged)
        Q_PROPERTY(int lineCount READ lineCount WRITE setLineCount NOTIFY lineCountChanged)

        Q_OBJECT
    public:
        explicit ConsoleLogger();

        static void customMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg);

        QString output();
        void setOutput(const QString & output);

        QString logFile();
        void setLogFile(const QString & logFile);

        int lineCount();
        void setLineCount(const int & lineCount);

    private:
        static QString _output;

        QString _logFile;

        static int _lineCount;

        void writeToFile(const QString & output);

    signals:
        void outputChanged();
        void lineCountChanged();
        void logFileChanged();
    };
}

#endif // CONSOLEVIEWER_H
