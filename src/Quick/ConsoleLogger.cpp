#include "Quick/ConsoleLogger.h"

Quick::ConsoleLogger * _instance = nullptr;

namespace Quick {
    QString ConsoleLogger::_output;

    ConsoleLogger::ConsoleLogger() {
        if (!_instance) {
            _instance = this;
        }
    }

    void ConsoleLogger::customMessageHandler(QtMsgType type, const QMessageLogContext & , const QString & msg) {
        switch (type) {
           case QtDebugMsg:
              _output += QString("<p>[Debug] \t\t %1</p>").arg(msg);
              break;
           case QtWarningMsg:
              _output += QString("<p><font color='yellow'>[Warning] \t %1</font></p>").arg(msg);
              break;
           case QtCriticalMsg:
              _output += QString("<p><font color='red'>[Critical] \t %1</font></p>").arg(msg);
              break;
           case QtFatalMsg:
              _output += QString("<p><font color='red'>[Fatal] \t\t %1</font></p>").arg(msg);
              abort();
              break;
        }

        if (_instance) {
            _instance->outputChanged();
        }
    }

    QString ConsoleLogger::output() {
        return _output;
    }

    void ConsoleLogger::setOutput(const QString & output) {
        _output = output;

        emit outputChanged();
    }

    int ConsoleLogger::lineCount() {
        return _lineCount;
    }

    void ConsoleLogger::setLineCount(const int & lineCount) {
        _lineCount = lineCount;

        emit lineCountChanged();
    }
}
