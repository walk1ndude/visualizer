#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmStringFilter.h>

#include <QtCore/QDebug>

#include "dicomreader.h"

#define WINDOW_BACKPROJECT_IMAGE "backprojet"
#define WINDOW_INPUT_IMAGE "input"
#define WINDOW_RADON "sinogram"
#define WINDOW_DHT "fourier1d"

DicomReader::DicomReader(QObject * parent) :
    QObject(parent),
    _imageNumber(0) {
 /*   if (initOpenCL()) {
        std::cerr << "OpenCL is not initialized... aborted" << std::endl;
        exit(0);
    }*/
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
                    catch (cv::Exception &){
                        continue;
                    }
                }
            }
        }
    }
    return OPENCL_NOT_INITIALIZED;
}

DicomReader::~DicomReader() {
    reset(_images);
    delete [] mergedData;
}

void DicomReader::resetV(std::vector<cv::Mat*> & vec, const int & newSize) {
    qDeleteAll(vec.begin(), vec.end());
    vec.clear();
    vec.resize(newSize);
}

void DicomReader::reset(Images & images, const int & newSize) {
    resetV(images.ctImages, newSize);
    resetV(images.images, newSize);
    resetV(images.sinograms, newSize);
    resetV(images.fourier1d, newSize);
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

    int imagesCount = dImage.GetDimension(2);

    dImage.GetBuffer(buffer);

    //clear previous "garbage"
    reset(_images, imagesCount);

    ctData.images = &_images;

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

    ctData.width = dImage.GetDimension(0);
    ctData.height = dImage.GetDimension(1);

    try {
        ctData.slope = dImage.GetSlope();
        ctData.intercept = dImage.GetIntercept();
    }
    catch(...) {
        ctData.slope = 1.0;
        ctData.intercept = 0.0;
    }

    ctData.buffer = buffer;

    qDebug() << "processing start";

    //cv::ocl::oclMat * oclData = new cv::ocl::oclMat(500, 500, CV_16UC1);
    //cv::ocl::GaussianBlur(*oclData, *oclData, cv::Size(5, 5), 5);

    cv::parallel_for_(cv::Range(0, imagesCount), CtProcessing<u_int16_t>(ctData));

    qDebug() << "loading done";

    cv::namedWindow(WINDOW_INPUT_IMAGE, cv::WINDOW_AUTOSIZE);
  /*  cv::namedWindow(WINDOW_BACKPROJECT_IMAGE, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(WINDOW_RADON, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(WINDOW_DHT, cv::WINDOW_AUTOSIZE);
*/
    medianSmooth(2);
    //expSmooth(0.7);
    mergeMatData(imageSpacings);

    showImageWithNumber(0);
}

void DicomReader::medianSmooth(const size_t & neighbours) {
    std::vector<cv::Mat *>medians;

    size_t size = _images.ctImages.size();

    cv::Mat mergeMat(_images.ctImages[0]->cols, _images.ctImages[0]->rows, CV_8UC1);

    for (size_t i = 0; i != size; ++ i) {
        if (i < neighbours || i > size - neighbours - 1) {
           medians.push_back(_images.ctImages[i]);
        }
        else {
            mergeMat = cv::Scalar(0);
            for (size_t j = i - neighbours; j != i + neighbours + 1; ++ j) {
                mergeMat += *(_images.ctImages[j]);
            }
            medians.push_back(new cv::Mat(mergeMat / (2 * neighbours + 1)));
        }
    }

    for (size_t i = 0; i != size; ++ i) {
        _images.ctImages[i] = medians[i];
    }

    _images.ctImages.erase(_images.ctImages.begin(), _images.ctImages.begin() + neighbours);
    _images.ctImages.erase(_images.ctImages.end() - neighbours, _images.ctImages.end());
}

void DicomReader::expSmooth(const float & alpha) {
    std::vector<cv::Mat *>medians;

    size_t size = _images.ctImages.size();

    cv::Mat mergeMat(_images.ctImages[0]->cols, _images.ctImages[0]->rows, CV_8UC1);

    for (size_t i = 0; i != size; ++ i) {
        if (i == 0) {
           mergeMat = (5 * (*(_images.ctImages[0])) + 2 * (*(_images.ctImages[1])) - (*(_images.ctImages[2]))) / 6;
        }
        else if (i == size - 1) {
           mergeMat = (5 * (*(_images.ctImages[i])) + 2 * (*(_images.ctImages[i - 1])) - (*(_images.ctImages[i - 2]))) / 6;
        }
        else {
            mergeMat = alpha * (*(_images.ctImages[i])) + (1 - alpha) * mergeMat;
        }
        medians.push_back(new cv::Mat(mergeMat));
    }
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

void DicomReader::mergeMatData(const std::vector<float> & imageSpacings) {
    cv::Mat * image = _images.ctImages[0];
    int z = _images.ctImages.size();

    int byteSizeMat = image->elemSize() * image->total();
    int byteSizeAll = byteSizeMat * z;

    mergedData = new uchar[byteSizeAll];

    for (int i = 0; i != z; ++ i) {
        memcpy(mergedData + byteSizeMat * i, _images.ctImages[i]->data, byteSizeMat);
    }

    std::vector<float> scaling;
    scaling.push_back(image->cols * imageSpacings[0] / (image->cols * imageSpacings[0]) / 0.7);
    scaling.push_back(image->rows * imageSpacings[1] / (image->cols * imageSpacings[0]) / 0.7);
    scaling.push_back(_images.ctImages.size() * imageSpacings[2] / (image->cols * imageSpacings[0]) / 0.7);

    std::vector<size_t>size;
    size.push_back(image->rows);
    size.push_back(image->cols);
    size.push_back(z);

    emit slicesProcessed(mergedData, scaling, size, ((image->step & 3) ? 1 : 4), image->step1());
}

void DicomReader::changeSliceNumber(int ds) {
    _imageNumber += ds;
    _imageNumber %= _images.ctImages.size();
    showImageWithNumber(_imageNumber);
}

void DicomReader::showImageWithNumber(const size_t & imageNumber) {
    cv::imshow(WINDOW_INPUT_IMAGE, *(_images.ctImages[imageNumber]));
    /*cv::imshow(WINDOW_BACKPROJECT_IMAGE, *((*_images).images[imageNumber]));

    cv::imshow(WINDOW_RADON, *((*_images).sinograms[imageNumber]));
    cv::imshow(WINDOW_DHT, *((*_images).fourier1d[imageNumber]));
*/
    cv::waitKey(1);
}
