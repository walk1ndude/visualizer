#ifndef STLREADER_H
#define STLREADER_H

#include <QtCore/QFile>

#include "Model/StlModel.h"

namespace Parser {
    class StlReader : public QObject {
        Q_OBJECT
    public:
        explicit StlReader(QObject * parent = 0);
        ~StlReader();

    private:
        void readASCII(QFile & stlFile);
        void readBinary(QFile & stlFile);

    signals:
        void readingErrorHappened();
        void modelRead(const ModelInfo::BuffersVN & buffers);

    public slots:
        void readFile(const QUrl & fileUrl);
    };
}
#endif // STLREADER_H
