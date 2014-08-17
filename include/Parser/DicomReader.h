#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <gdcmImage.h>
#include <gdcmFile.h>

#include <QtQuick/QQuickItem>

#include "Parser/ctprocessing.hpp"

#include "Info/SliceInfo.h"

namespace Parser {
    class DicomReader : public QQuickItem {
        Q_PROPERTY(QUrl dicomFile READ dicomFile WRITE setDicomFile NOTIFY dicomFileChanged)

        Q_OBJECT
    public:
        explicit DicomReader();
        ~DicomReader();

        void reset(const int & newSize = 0);
        void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

        QUrl dicomFile() const;

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

    signals:
        void slicesProcessed(QVariant slices);

        void dicomFileChanged();

    public slots:
        Q_INVOKABLE void nextSlice(const int & ds);

        void setDicomFile(const QUrl & dicomFile);

        void updateMinHU(const int & minHU);
        void updateMaxHU(const int & maxHU);
    };
}

#endif // DICOMREADER_H
