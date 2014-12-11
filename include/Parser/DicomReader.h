#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <gdcmImage.h>
#include <gdcmFile.h>

#include "Parser/ctprocessing.hpp"
#include "Parser/AbstractParser.h"

#include "Info/VolumeInfo.h"

namespace Parser {
class DicomReader : public AbstractParser {
    Q_OBJECT
public:
    explicit DicomReader();

    QUrl file() const;

private:
    QUrl _dicomFile;

    DicomData _dicomData;

    void readImage(gdcm::File &dFile, const gdcm::Image & dImage);

    void fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage);
    void runSliceProcessing(const bool & tellAboutHURange = false);

public slots:
    virtual void setFile(const QUrl & file);
};
}

#endif // DICOMREADER_H
