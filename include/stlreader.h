#ifndef STLREADER_H
#define STLREADER_H

#include <QtCore/QFile>

#include "modelinfo.h"

class StlReader : public QObject {
    Q_OBJECT
public:
    explicit StlReader(QObject * parent = 0);
    ~StlReader();

private:
    void readASCII(QFile & fileName);
    void readBinary(QFile & fileName);

    QString removeWhitespaes(QString & str);

signals:
    void readingErrorHappened();
    void modelRead(const ModelInfo::ModelBuffers & modelBuffers);

public slots:
    void readFile(const QUrl & fileUrl);
};

#endif // STLREADER_H
