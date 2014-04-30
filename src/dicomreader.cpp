#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>
#include <gdcmException.h>

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#include "dicomreader.h"

#include <opencv2/highgui/highgui.hpp>

#define WINDOW_NOISY "noisy"

#define MIN_HU 800
#define MAX_HU 3600

DicomReader::DicomReader(QObject * parent) :
    QObject(parent),
    _imageNumber(0) {

}

DicomReader::~DicomReader() {
    reset();
}

void DicomReader::resetV(std::vector<cv::Mat *> & vec, const int & newSize) {
    for (std::vector<cv::Mat *>::iterator it = vec.begin(); it != vec.end(); ++ it) {
        delete *it;
    }
    vec.clear();
    vec.resize(newSize);
}

void DicomReader::reset(const int & newSize) {
    resetV(_noisy, newSize);
}


void DicomReader::fetchDicomData(DicomData & dicomData, gdcm::File & dFile, const gdcm::Image & dImage) {
    gdcm::StringFilter dStringFilter;
    dStringFilter.SetFile(dFile);

    gdcm::DataSet & dDataSet = dFile.GetDataSet();

    for (int i = 0; i != 2; ++ i) {
        dicomData.imageSpacings.push_back(dImage.GetSpacing(i));
    }

    gdcm::Tag tagFind(0x0018, 0x0050);
    if (dDataSet.FindDataElement(tagFind)) {
        dicomData.imageSpacings.push_back(std::stof(dStringFilter.ToString(tagFind)));
    }
    else {
        //pure assumtion for now
        dicomData.imageSpacings.push_back(1.0);
    }

    tagFind.SetElementTag(0x0018, 0x0088);
    if (dDataSet.FindDataElement(tagFind)) {
        dicomData.imageSpacings[2] += std::stof(dStringFilter.ToString(tagFind));
    }
    else {
        //pure assumtion for now
        dicomData.imageSpacings[2] += 0.1;
    }

    tagFind.SetElementTag(0x0028, 0x1050);
    if (dDataSet.FindDataElement(tagFind)) {
        dicomData.windowCenter = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        dicomData.windowCenter = 0;
    }

    tagFind.SetElementTag(0x0028, 0x1051);
    if (dDataSet.FindDataElement(tagFind)) {
        dicomData.windowWidth = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        dicomData.windowWidth = 0;
    }

    dicomData.width = dImage.GetDimension(0);
    dicomData.height = dImage.GetDimension(1);
    dicomData.depth = dImage.GetDimension(2);

    dicomData.vbuffer.resize(dImage.GetBufferLength());
    dicomData.buffer = &(dicomData.vbuffer[0]);
    dImage.GetBuffer(dicomData.buffer);

    //MONOCHROME2

    gdcm::PhotometricInterpretation photometricInterpretation = dImage.GetPhotometricInterpretation();

    if (photometricInterpretation != gdcm::PhotometricInterpretation::MONOCHROME2) {
        dicomData.inverseNeeded = true;
    }
    else {
        dicomData.inverseNeeded = false;
    }

    gdcm::PixelFormat pixelFormat = dImage.GetPixelFormat();
    if (pixelFormat.GetScalarType() == gdcm::PixelFormat::UINT16) {
        dicomData.type = CV_16UC1;
    }
    else {
        dicomData.type = CV_8UC1; // for now
    }

    dicomData.bytesAllocated = pixelFormat.GetBitsAllocated() / 8;

    try {
        dicomData.slope = dImage.GetSlope();
        dicomData.intercept = dImage.GetIntercept();
    }
    catch (gdcm::Exception &) {
        dicomData.slope = 1.0;
        dicomData.intercept = 0.0;
    }

    dicomData.minValue = pixelFormat.GetMin();
    dicomData.maxValue = pixelFormat.GetMax();

    dicomData.minHUPossible = dicomData.slope * dicomData.minValue + dicomData.intercept;
    dicomData.maxHUPossible = dicomData.slope * dicomData.maxValue + dicomData.intercept;

    dicomData.isLittleEndian = (pixelFormat.GetBitsAllocated() - pixelFormat.GetHighBit() == 1) ? true : false;

    dicomData.neighbourRadius = 2;

    dicomData.noisy = &_noisy;
    dicomData.filtered = &_filtered;

    dicomData.minHU = std::max(dicomData.minHUPossible, MIN_HU);
    dicomData.maxHU = std::min(dicomData.maxHUPossible, MAX_HU);
}

void DicomReader::readImage(gdcm::File & dFile, const gdcm::Image & dImage) {

    fetchDicomData(_dicomData, dFile, dImage);

    cv::namedWindow(WINDOW_NOISY, CV_WINDOW_AUTOSIZE);

    runSliceProcessing(true);
}

void DicomReader::runSliceProcessing(const bool & tellAboutHURange) {
    uchar * mergedData = 0;

    int alignment = 0;
    size_t rowLength = 0;

    _dicomData.alignment = &alignment;
    _dicomData.rowLenght = &rowLength;

    _dicomData.mergeLocation = &mergedData;

    qint64 procTime = QDateTime::currentMSecsSinceEpoch();

    cv::parallel_for_(cv::Range(0, _dicomData.depth), SliceProcessing(&_dicomData));

    procTime = QDateTime::currentMSecsSinceEpoch() - procTime;

    qDebug() << "finished" << procTime;

    std::vector<float>scaling;
    std::vector<size_t>size;

    int depth = _dicomData.depth - _dicomData.neighbourRadius * 2;

    scaling.push_back(_dicomData.width * _dicomData.imageSpacings[0] / (_dicomData.height * _dicomData.imageSpacings[0]) / 0.7);
    scaling.push_back(_dicomData.height * _dicomData.imageSpacings[1] / (_dicomData.height * _dicomData.imageSpacings[0]) / 0.7);
    scaling.push_back(depth * _dicomData.imageSpacings[2] / (_dicomData.height * _dicomData.imageSpacings[0]) / 0.7);

    size.push_back(_dicomData.width);
    size.push_back(_dicomData.height);
    size.push_back(depth);

    std::vector<int>huRange;

    if (tellAboutHURange) {
        huRange.push_back(_dicomData.minHUPossible);
        huRange.push_back(_dicomData.maxHUPossible);
    }

    emit slicesProcessed(QSharedPointer<uchar>(mergedData), scaling, size, alignment, rowLength, huRange);
}

void DicomReader::readFile(const QString & dicomFile) {
    gdcm::ImageReader dIReader;

    // dicomFile = "file:///...", we must cut protocol, so no "file://" <- start with 7th char
#if defined(_WIN32)
    dIReader.SetFileName(dicomFile.mid(8).toStdString().c_str());
#else
    dIReader.SetFileName(dicomFile.mid(7).toStdString().c_str());
#endif
    if (dIReader.Read()) {
        readImage(dIReader.GetFile(), dIReader.GetImage());
    }
    else {
        qDebug() << "can't read file";
    }
}

void DicomReader::changeSliceNumber(const int & ds) {
    _imageNumber += ds;
    _imageNumber %= _noisy.size();
    showImageWithNumber(_imageNumber);
}

void DicomReader::showImageWithNumber(const size_t & imageNumber) {
    cv::imshow(WINDOW_NOISY, *(_noisy[imageNumber]));
    cv::waitKey(1);
}

void DicomReader::updateMinHU(const int & minHU) {
    _dicomData.minHU = std::min(minHU, _dicomData.maxHU);
    _dicomData.maxHU = std::max(minHU, _dicomData.maxHU);

    resetV(_noisy);
    resetV(_filtered);

    runSliceProcessing();
}

void DicomReader::updateMaxHU(const int & maxHU) {
    _dicomData.minHU = std::max(maxHU, _dicomData.minHU);
    _dicomData.maxHU = std::min(maxHU, _dicomData.minHU);

    resetV(_noisy);
    resetV(_filtered);

    runSliceProcessing();
}
