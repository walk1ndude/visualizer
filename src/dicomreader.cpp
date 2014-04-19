#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>
#include <gdcmException.h>

#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#include "dicomreader.h"

#define WINDOW_BACKPROJECT_IMAGE "backprojet"
#define WINDOW_INPUT_IMAGE "input"
#define WINDOW_RADON "sinogram"
#define WINDOW_DHT "fourier1d"

DicomReader::DicomReader(QObject * parent) :
    QObject(parent),
    _imageNumber(0),
    _minValue(40),
    _maxValue(55) {
   if (initOpenCL()) {
        std::cerr << "OpenCL is not initialized... aborted" << std::endl;
        exit(0);
    }
}

int DicomReader::initOpenCL() {
    cv::ocl::PlatformsInfo platforms;

    if (cv::ocl::getOpenCLPlatforms(platforms)) {

        cv::ocl::DevicesInfo devices;

        cv::ocl::DeviceType deviceType[2] = {cv::ocl::CVCL_DEVICE_TYPE_GPU, cv::ocl::CVCL_DEVICE_TYPE_CPU};

        // for now just take the first OpenCL capable GPU or CPU

        for (uint i = 0; i < 2; i ++) {
            for (uint platformN = 0; platformN < platforms.size(); platformN ++) {

                if (cv::ocl::getOpenCLDevices(devices, deviceType[i], platforms[platformN])) {
                    try {
                        cv::ocl::setDevice(devices[0]);

                        std::cout << cv::ocl::Context::getContext()->getDeviceInfo().deviceName << " " <<
                                    cv::ocl::Context::getContext()->getDeviceInfo().deviceProfile << std::endl;

                        return OPENCL_ALL_OK;
                    }
                    catch (cv::Exception &) {
                        continue;
                    }
                }
            }
        }
    }
    return OPENCL_NOT_INITIALIZED;
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

void DicomReader::readImage(gdcm::File & dFile, const gdcm::Image & dImage) {
    std::vector<char>vbuffer;
    vbuffer.resize(dImage.GetBufferLength());
    char * buffer = &vbuffer[0];

    gdcm::StringFilter dStringFilter;
    dStringFilter.SetFile(dFile);

    gdcm::DataSet & dDataSet = dFile.GetDataSet();

    std::vector<float>imageSpacings;

    for (int i = 0; i != 2; ++ i) {
        imageSpacings.push_back(dImage.GetSpacing(i));
    }

    gdcm::Tag tagFind(0x0018, 0x0050);
    if (dDataSet.FindDataElement(tagFind)) {
        imageSpacings.push_back(std::stof(dStringFilter.ToString(tagFind)));
    }
    else {
        //pure assumtion for now
        imageSpacings.push_back(1.0);
    }

    tagFind.SetElementTag(0x0018, 0x0088);
    if (dDataSet.FindDataElement(tagFind)) {
        imageSpacings[2] += std::stof(dStringFilter.ToString(tagFind));
    }
    else {
        //pure assumtion for now
        imageSpacings[2] += 0.1;
    }

    LoaderData loaderData;

    tagFind.SetElementTag(0x0028, 0x1050);
    if (dDataSet.FindDataElement(tagFind)) {
        loaderData.dicomData.windowCenter = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        loaderData.dicomData.windowCenter = 0;
    }

    tagFind.SetElementTag(0x0028, 0x1051);
    if (dDataSet.FindDataElement(tagFind)) {
        loaderData.dicomData.windowWidth = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        loaderData.dicomData.windowWidth = 0;
    }

    loaderData.dicomData.width = dImage.GetDimension(0);
    loaderData.dicomData.height = dImage.GetDimension(1);
    loaderData.dicomData.depth = dImage.GetDimension(2);

    dImage.GetBuffer(buffer);

    loaderData.filterData.src = &_noisy;

    loaderData.filterData.minValue = _minValue;
    loaderData.filterData.maxValue = _maxValue;

    loaderData.dicomData.imageSpacing = &imageSpacings;

    loaderData.dicomData.isRadonNeeded = false;

    //MONOCHROME2

    gdcm::PhotometricInterpretation photometricInterpretation = dImage.GetPhotometricInterpretation();

    if (photometricInterpretation == gdcm::PhotometricInterpretation::MONOCHROME2) {
        loaderData.dicomData.inverseNeeded = true;
    }
    else {
        loaderData.dicomData.inverseNeeded = false;
    }

    gdcm::PixelFormat pixelFormat = dImage.GetPixelFormat();
    if (pixelFormat.GetScalarType() == gdcm::PixelFormat::UINT16) {
        loaderData.dicomData.type = CV_16UC1;
    }
    else {
        loaderData.dicomData.type = CV_8UC1; // for now
    }

    loaderData.dicomData.bytesAllocated = pixelFormat.GetBitsAllocated() / 8;

    loaderData.dicomData.minValue = pixelFormat.GetMin();
    loaderData.dicomData.maxValue = pixelFormat.GetMax();

    loaderData.dicomData.isLittleEndian = (pixelFormat.GetBitsAllocated() - pixelFormat.GetHighBit() == 1) ? true : false;

    try {
        loaderData.dicomData.slope = dImage.GetSlope();
        loaderData.dicomData.intercept = dImage.GetIntercept();
    }
    catch (gdcm::Exception &) {
        loaderData.dicomData.slope = 1.0;
        loaderData.dicomData.intercept = 0.0;
    }

    loaderData.dicomData.buffer = buffer;

    loaderData.filterData.minValue = _minValue;
    loaderData.filterData.maxValue = _maxValue;

    uchar * mergedData;

    size_t rowLength;

    int aligment;

    loaderData.filterData.mergeLocation = &mergedData;

    loaderData.filterData.rowLenght = &rowLength;
    loaderData.filterData.alignment = &aligment;

    loaderData.filterData.neighbourRadius = 2;

    qDebug() << "processing start";

    qint64 procTime = QDateTime::currentMSecsSinceEpoch();

    cv::parallel_for_(cv::Range(0, loaderData.dicomData.depth), SliceLoader<u_int16_t>(loaderData));

    procTime = QDateTime::currentMSecsSinceEpoch() - procTime;

    qDebug() << "loading done" << procTime;

    //cv::namedWindow(WINDOW_INPUT_IMAGE, CV_WINDOW_AUTOSIZE);

    std::vector<float>scaling;
    std::vector<size_t>size;

    cv::Mat * image = _noisy.at(0);

    int depth = loaderData.dicomData.depth - loaderData.filterData.neighbourRadius * 2;

    scaling.push_back(image->cols * imageSpacings[0] / (image->cols * imageSpacings[0]) / 0.7);
    scaling.push_back(image->rows * imageSpacings[1] / (image->cols * imageSpacings[0]) / 0.7);
    scaling.push_back(depth * imageSpacings[2] / (image->cols * imageSpacings[0]) / 0.7);

    size.push_back(image->rows);
    size.push_back(image->cols);
    size.push_back(depth);

    emit slicesProcessed(mergedData, scaling, size, aligment, rowLength);
}

void DicomReader::readFile(QString dicomFile) {
    gdcm::ImageReader dIReader;

    // dicomFile = "file:///...", we must cut protocol, so no "file://" <- start with 7th char
    dIReader.SetFileName(dicomFile.mid(7).toStdString().c_str());
    if (dIReader.Read()) {
        readImage(dIReader.GetFile(), dIReader.GetImage());
    }
    else {
        qDebug() << "can't read file";
    }
}

void DicomReader::updateFiltered() {
    FilterData filterData;

    filterData.src = &_noisy;

    filterData.minValue = _minValue;
    filterData.maxValue = _maxValue;

    filterData.neighbourRadius = 2;

    uchar * mergedData = 0;

    int alignment = 0;
    size_t rowLength = 0;

    filterData.alignment = &alignment;
    filterData.rowLenght = &rowLength;

    filterData.mergeLocation = &mergedData;

    qint64 procTime = QDateTime::currentMSecsSinceEpoch();

    cv::parallel_for_(cv::Range(0, _noisy.size()), SliceFilter(filterData));

    procTime = QDateTime::currentMSecsSinceEpoch() - procTime;

    qDebug() << "finished" << procTime;

    std::vector<float>scaling;
    std::vector<size_t>size;

    emit slicesProcessed(mergedData, scaling, size, alignment, rowLength);
}

void DicomReader::changeSliceNumber(int ds) {
    _imageNumber += ds;
    _imageNumber %= _noisy.size();
    showImageWithNumber(_imageNumber);
}

void DicomReader::showImageWithNumber(const size_t & imageNumber) {
    cv::imshow(WINDOW_INPUT_IMAGE, *(_noisy[imageNumber]));
    cv::waitKey(1);
}

void DicomReader::updateMinValue(int minValue) {
    _minValue = std::min(minValue, _maxValue);
    _maxValue = std::max(minValue, _maxValue);

    updateFiltered();
}

void DicomReader::updateMaxValue(int maxValue) {
    _maxValue = std::max(maxValue, _minValue);
    _minValue = std::min(maxValue, _minValue);

    updateFiltered();
}
