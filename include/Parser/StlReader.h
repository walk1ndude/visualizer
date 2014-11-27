#ifndef STLREADER_H
#define STLREADER_H

#include "Parser/Helpers.hpp"
#include "Parser/AbstractParser.h"

#include "Model/VertexVN.h"

namespace Parser {
    class StlReader : public AbstractParser {
        Q_OBJECT
    public:
        explicit StlReader();
        ~StlReader();

        QUrl file() const;

    private:
        QUrl _stlFile;

        void readASCII(QFile & stlFile, const bool & solidChecked = false);
        void readBinary(QFile & stlFile);

        void sendBuffers(ModelInfo::BuffersVN buffers);

    signals:
        void readingErrorHappened();

    public slots:
        virtual void setFile(const QUrl & file);
    };
}
#endif // STLREADER_H
