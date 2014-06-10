#ifndef STLREADER_H
#define STLREADER_H

#include <QtCore/QFile>

#include "Info/Modelinfo.h"

class StlReader : public QObject {
    Q_OBJECT
public:
    explicit StlReader(QObject * parent = 0);
    ~StlReader();

private:
    void readASCII(QFile & stlFile);
    void readBinary(QFile & stlFile);

    QString removeWhitespaes(QString & str);

signals:
    void readingErrorHappened();
    void modelRead(const ModelInfo::BuffersVN & buffers);

public slots:
    void readFile(const QUrl & fileUrl);
};

#endif // STLREADER_H
