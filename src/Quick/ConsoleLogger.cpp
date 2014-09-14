#include <QtCore/QRegExp>

#include "Quick/ConsoleLogger.h"

Quick::ConsoleLogger * _instance = nullptr;

QRegExp logRx("<p>([^<]*)</p>");

namespace Quick {
    QString ConsoleLogger::_output;

    int ConsoleLogger::_lineCount = 5;

    ConsoleLogger::ConsoleLogger() {
        if (!_instance) {
            _instance = this;
        }
    }

    void ConsoleLogger::customMessageHandler(QtMsgType type, const QMessageLogContext & , const QString & msg) {
        QString appendStr;

        switch (type) {
           case QtDebugMsg:
              appendStr = QString("[Debug] \t\t %1").arg(msg);
              break;
           case QtWarningMsg:
              appendStr = QString("[Warning] \t %1").arg(msg);
              break;
           case QtCriticalMsg:
              appendStr = QString("[Critical] \t %1").arg(msg);
              break;
           case QtFatalMsg:
              appendStr = QString("[Fatal] \t\t %1").arg(msg);
              break;
        }

        if (_instance) {
            _instance->writeToFile(appendStr);
        }

        switch (type) {
           case QtWarningMsg:
              appendStr = QString("<font color='yellow'>") + appendStr + QString("</font>");
              break;
           case QtCriticalMsg:
              appendStr = QString("<font color='red'>") + appendStr + QString("</font>");
              break;
           case QtFatalMsg:
              appendStr = QString("<font color='red'>") + appendStr + QString("</font>");
              abort();
           default:
            break;
        }

        _output += (QString("<p>") + appendStr + QString("</p>"));

        int pos = 0;
        int position = 0;
        int count = 0;

        while (count != _lineCount && (pos = logRx.indexIn(_output, pos)) != -1) {
            count ++;
            pos += logRx.matchedLength();

            if (!position) {
                position = pos;
            }
        }

        _output = _output.mid(count == _lineCount - 1 ? position : 0);

        if (_instance) {
            _instance->outputChanged();
        }
    }

    QString ConsoleLogger::output() const {
        return _output;
    }

    void ConsoleLogger::setOutput(const QString & output) {
        _output = output;

        emit outputChanged();
    }

    QString ConsoleLogger::logFile() const {
        return _logFile;
    }

    void ConsoleLogger::setLogFile(const QString & logFile) {
        _logFile = logFile;

        emit logFileChanged();
    }

    void ConsoleLogger::writeToFile(const QString & output) {
        if (!_logFile.isEmpty()) {
            QFile outFile(_logFile);
            outFile.open(QIODevice::WriteOnly | QIODevice::Append);

            QTextStream textStream(&outFile);
            textStream << output << endl;

            outFile.close();
        }
    }

    int ConsoleLogger::lineCount() const {
        return _lineCount;
    }

    void ConsoleLogger::setLineCount(const int & lineCount) {
        _lineCount = lineCount;

        emit lineCountChanged();
    }
}
