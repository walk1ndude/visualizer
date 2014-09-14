#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <gdcmImage.h>
#include <gdcmFile.h>

#include "Parser/ctprocessing.hpp"

#include "Info/SliceInfo.h"

namespace Parser {
    class DicomReader : public QQuickItem {
        Q_PROPERTY(QUrl dicomFile READ dicomFile WRITE setDicomFile NOTIFY dicomFileChanged)

        Q_OBJECT
    public:
        explicit DicomReader();
        ~DicomReader();

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

        void reset(const int & newSize = 0);
        void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

    signals:
        void slicesProcessed(QVariant slices);

        void dicomFileChanged();

    public slots:
        Q_INVOKABLE virtual void nextSlice(const int & ds);

        virtual void setDicomFile(const QUrl & dicomFile) final;

        virtual void updateMinHU(const int & minHU);
        virtual void updateMaxHU(const int & maxHU);
    };
}

#endif // DICOMREADER_H
