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
        ~DicomReader();

        QUrl file() const;

    private:
        size_t _sliceNumber;

        QUrl _dicomFile;

        std::vector<cv::Mat *> _noisy;
        std::vector<cv::Mat *> _filtered;

        DicomData _dicomData;

        void showSliceWithNumber(const size_t & sliceNumber);
        void readImage(gdcm::File &dFile, const gdcm::Image & dImage);

        void fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage);
        void runSliceProcessing(const bool & tellAboutHURange = false);
        void updateDicomData();

        void reset(const int & newSize = 0);
        void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

    public slots:
        Q_INVOKABLE virtual void nextSlice(const int & ds);

        virtual void setFile(const QUrl & file);

        virtual void updateMinHU(const int & minHU);
        virtual void updateMaxHU(const int & maxHU);
    };
}

#endif // DICOMREADER_H
