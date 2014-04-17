#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>

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
    qDeleteAll(vec.begin(), vec.end());
    vec.clear();
    vec.resize(newSize);
}

void DicomReader::reset(const int & newSize) {
    resetV(_noisy, newSize);
    resetV(_filtered, newSize);
    resetV(_smoothed, newSize);
}

void DicomReader::readImage(gdcm::File & dFile, const gdcm::Image & dImage) {
    std::vector<char>vbuffer;
    vbuffer.resize(dImage.GetBufferLength());
    char * buffer = &vbuffer[0];

    gdcm::StringFilter dStringFilter;
    dStringFilter.SetFile(dFile);

    gdcm::DataSet & dDataSet = dFile.GetDataSet();

    CtData ctData;

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

    tagFind.SetElementTag(0x0028, 0x1050);
    if (dDataSet.FindDataElement(tagFind)) {
        ctData.windowCenter = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        ctData.windowCenter = 0;
    }

    tagFind.SetElementTag(0x0028, 0x1051);
    if (dDataSet.FindDataElement(tagFind)) {
        ctData.windowWidth = std::stoi(dStringFilter.ToString(tagFind));
    }
    else {
        ctData.windowWidth = 0;
    }

    ctData.width = dImage.GetDimension(0);
    ctData.height = dImage.GetDimension(1);
    ctData.depth = dImage.GetDimension(2);

    dImage.GetBuffer(buffer);

    //clear previous "garbage"
    reset(ctData.depth);

    ctData.noisy = &_noisy;
    ctData.filtered = &_filtered;

    ctData.minValueRange = _minValue;
    ctData.maxValueRange = _maxValue;

    ctData.imageSpacing = &imageSpacings;

    ctData.isRadonNeeded = false;

    //MONOCHROME2

    gdcm::PhotometricInterpretation photometricInterpretation = dImage.GetPhotometricInterpretation();

    if (photometricInterpretation == gdcm::PhotometricInterpretation::MONOCHROME2) {
        ctData.inverseNeeded = true;
    }
    else {
        ctData.inverseNeeded = false;
    }

    gdcm::PixelFormat pixelFormat = dImage.GetPixelFormat();
    if (pixelFormat.GetScalarType() == gdcm::PixelFormat::UINT16) {
        ctData.type = CV_16UC1;
    }
    ctData.bytesAllocated = pixelFormat.GetBitsAllocated() / 8;
    ctData.minValue = pixelFormat.GetMin();
    ctData.maxValue = pixelFormat.GetMax();

    ctData.isLittleEndian = (pixelFormat.GetBitsAllocated() - pixelFormat.GetHighBit() == 1) ? true : false;

    try {
        ctData.slope = dImage.GetSlope();
        ctData.intercept = dImage.GetIntercept();
    }
    catch(...) {
        ctData.slope = 1.0;
        ctData.intercept = 0.0;
    }

    ctData.buffer = buffer;

    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    qDebug() << "processing start";

    cv::parallel_for_(cv::Range(0, ctData.depth), CtProcessing<u_int16_t>(ctData));

    qDebug() << "loading done" << QDateTime::currentMSecsSinceEpoch() - startTime;

    cv::namedWindow(WINDOW_INPUT_IMAGE, cv::WINDOW_AUTOSIZE);

    startTime = QDateTime::currentMSecsSinceEpoch();

    medianSmooth(2);
    mergeMatData(_smoothed, imageSpacings);

    qDebug() << "merge finished" << QDateTime::currentMSecsSinceEpoch() - startTime;

    showImageWithNumber(0);
}

void DicomReader::medianSmooth(const size_t & neighbourRadius) {
    resetV(_smoothed, _filtered.size() - 2 * neighbourRadius);

    SmoothData smoothData;

    smoothData.src = &_filtered;
    smoothData.dst = &_smoothed;

    smoothData.neighbourRadius = neighbourRadius;

    cv::parallel_for_(cv::Range(0, _smoothed.size()), VolumeSmoother(smoothData));
}

void DicomReader::readFile(QString dicomFile) {
    //_mutex.lock();

    gdcm::ImageReader dIReader;

    // dicomFile = "file:///...", we must cut protocol, so no "file://" <- start with 7th char
    dIReader.SetFileName(dicomFile.mid(7).toStdString().c_str());
    if (dIReader.Read()) {
        readImage(dIReader.GetFile(), dIReader.GetImage());
        //_mutex.unlock();
    }
    else {
        qDebug() << "can't read file";
        //_mutex.unlock();
    }
}

void DicomReader::mergeMatData(const std::vector<cv::Mat *> & src, const std::vector<float> & imageSpacings) {
    cv::Mat * image = src[0];
    int depth = src.size();

    int byteSizeMat = image->elemSize() * image->total();
    int byteSizeAll = byteSizeMat * depth;

    uchar * mergedData = new uchar[byteSizeAll];

    for (int i = 0; i != depth; ++ i) {
        memcpy(mergedData + byteSizeMat * i, src[i]->data, byteSizeMat);
    }

    std::vector<float> scaling;
    std::vector<size_t>size;

    if (imageSpacings.size() > 0) {

        scaling.push_back(image->cols * imageSpacings[0] / (image->cols * imageSpacings[0]) / 0.7);
        scaling.push_back(image->rows * imageSpacings[1] / (image->cols * imageSpacings[0]) / 0.7);
        scaling.push_back(src.size() * imageSpacings[2] / (image->cols * imageSpacings[0]) / 0.7);

        size.push_back(image->rows);
        size.push_back(image->cols);
        size.push_back(depth);
    }

    emit slicesProcessed(mergedData, scaling, size, ((image->step & 3) ? 1 : 4), image->step1());
}

void DicomReader::updateFiltered() {
    FilterData filterData;

    filterData.src = _noisy;
    filterData.dst = _filtered;

    filterData.minValue = _minValue;
    filterData.maxValue = _maxValue;

    cv::parallel_for_(cv::Range(0, _noisy.size()), SliceFilter(filterData));

    medianSmooth(2);
    mergeMatData(_smoothed);
}

void DicomReader::changeSliceNumber(int ds) {
    _imageNumber += ds;
    _imageNumber %= _smoothed.size();
    showImageWithNumber(_imageNumber);
}

void DicomReader::showImageWithNumber(const size_t & imageNumber) {
    cv::imshow(WINDOW_INPUT_IMAGE, *(_smoothed[imageNumber]));
    cv::waitKey(1);
}

void DicomReader::updateMinValue(int minValue) {
    //_mutex.lock();

    _minValue = std::min(minValue, _maxValue);
    _maxValue = std::max(minValue, _maxValue);

    updateFiltered();

    //_mutex.unlock();
}

void DicomReader::updateMaxValue(int maxValue) {
    //_mutex.lock();

    _maxValue = std::max(maxValue, _minValue);
    _minValue = std::min(maxValue, _minValue);

    updateFiltered();

    //_mutex.unlock();
}
