#ifndef STLREADER_H
#define STLREADER_H

#include "Parser/Helpers.hpp"

#include "Model/StlModel.h"

namespace Parser {
    class StlReader : public QObject {
        Q_PROPERTY(QUrl stlFile READ stlFile WRITE setStlFile NOTIFY stlFileChanged)

        Q_OBJECT
    public:
        explicit StlReader();
        ~StlReader();

        QUrl stlFile() const;

    private:
        QUrl _stlFile;

        void readASCII(QFile & stlFile);
        void readBinary(QFile & stlFile);

    signals:
        void readingErrorHappened();
        void modelRead(QVariant buffers);

        void stlFileChanged();

    public slots:
        virtual void setStlFile(const QUrl & stlFile) final;
    };
}
#endif // STLREADER_H
