#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

#include "reconstructor.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define SLICES_IMAGE_WINDOW "slices"
#define SLICE_POSITION "position"

#define toRad(x) ((x) * CV_PI / 180.0)
#define PADDED_INCREASE 1

#define SCALE_COEFF ((float) 0.7)

Reconstructor::Reconstructor(QObject * parent) :
    QObject(parent),
    _sliceNumber(0) {

    initOpenCL();
}

Reconstructor::~Reconstructor() {
    reset();

    clReleaseKernel(_calcTrigKernel);
    clReleaseKernel(_calcCasKernel);
    clReleaseKernel(_butterflyDht2dKernel);
    clReleaseKernel(_fourier2dKernel);
    clReleaseKernel(_dht1dTransposeKernel);

    clReleaseProgram(_programSlice);
    clReleaseCommandQueue(_queue);
#ifdef CL_VERSION_1_2
    clReleaseDevice(_device_id);
#endif
    clReleaseContext(_context);
}

void Reconstructor::initOpenCL() {
    QFile programFile(":cl/reconstructor.cl");

    programFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stream(&programFile);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));

    QString programStringList;

    while (true) {
        QString line = stream.readLine();
        if (line.isNull()) {
            break;
        }
        else {
            programStringList.append(line + '\n');
        }
    }

    char * programText = (char *) malloc(programStringList.length());
    size_t programLength = strlen(programStringList.toStdString().c_str()) + 1;
    memcpy(programText, programStringList.toStdString().c_str(), programLength);

    programFile.close();

    _context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);

    clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &_device_id, NULL);
    _queue = clCreateCommandQueue(_context, _device_id, 0, NULL);

    _programSlice = clCreateProgramWithSource(_context, 1, (const char **) &programText,
                                              (const size_t *) &programLength, NULL);

    delete [] programText;

    qDebug() << "Building OpenCL Program, error: " << clBuildProgram(_programSlice, 1, &_device_id, NULL, NULL, NULL);

    _calcCasKernel = clCreateKernel(_programSlice, "calcCas", NULL);
    _calcTrigKernel = clCreateKernel(_programSlice, "calcTrig", NULL);
    _fourier2dKernel = clCreateKernel(_programSlice, "fourier2d", NULL);
    _dht1dTransposeKernel = clCreateKernel(_programSlice, "dht1dTranspose", NULL);
    _butterflyDht2dKernel = clCreateKernel(_programSlice, "butterflyDht2d", NULL);
}

void Reconstructor::reconstruct() {
    float startTime = cv::getTickCount() / cv::getTickFrequency();

    size_t height = _src.at(0).rows;
    size_t width = _src.at(0).cols;
    size_t depth = _src.size();
    size_t paddedWidth = width * PADDED_INCREASE;

    size_t rowPitchSrc = sizeof(float) * width;
    size_t slicePitchSrc = rowPitchSrc * height;
    size_t srcSize = slicePitchSrc * depth;

    size_t slicePitchFourier2d = sizeof(float) * paddedWidth * paddedWidth;

    size_t rowPitchDst = sizeof(float) * width;
    size_t slicePitchDst = rowPitchDst * width;

    uchar * srcData = new uchar[srcSize];

    for (size_t i = 0; i != depth; ++ i) {
        memcpy(srcData + slicePitchSrc * i, _src.at(i).data, slicePitchSrc);
    }

    cl_image_format image_format;
    image_format.image_channel_data_type = CL_FLOAT;
    image_format.image_channel_order = CL_R;

#ifdef CL_VERSION_1_2
    cl_image_desc image_desc_src;
    image_desc_src.image_type = CL_MEM_OBJECT_IMAGE3D;
    image_desc_src.image_width = width;
    image_desc_src.image_height = height;
    image_desc_src.image_depth = depth;
    image_desc_src.buffer = NULL;
    image_desc_src.num_mip_levels = 0;
    image_desc_src.image_row_pitch = rowPitchSrc;
    image_desc_src.image_slice_pitch = slicePitchSrc;
    image_desc_src.num_samples = 0;

    cl_image_desc image_desc_fourier2d;
    image_desc_fourier2d.image_type = CL_MEM_OBJECT_IMAGE3D;
    image_desc_fourier2d.image_width = paddedWidth;
    image_desc_fourier2d.image_height = paddedWidth;
    image_desc_fourier2d.image_depth = height;
    image_desc_fourier2d.buffer = NULL;
    image_desc_fourier2d.num_mip_levels = 0;
    image_desc_fourier2d.image_row_pitch = 0;
    image_desc_fourier2d.image_slice_pitch = 0;
    image_desc_fourier2d.num_samples = 0;

    cl_image_desc image_desc_slices;
    image_desc_slices.image_type = CL_MEM_OBJECT_IMAGE3D;
    image_desc_slices.image_width = width;
    image_desc_slices.image_height = width;
    image_desc_slices.image_depth = height;
    image_desc_slices.buffer = NULL;
    image_desc_slices.num_mip_levels = 0;
    image_desc_slices.image_row_pitch = 0;
    image_desc_slices.image_slice_pitch = 0;
    image_desc_slices.num_samples = 0;
#endif
    size_t origin[3] = {0, 0, 0};
    size_t regionSlice[3] = {width, width, height};

    cl_event eventList[7];

    int errNo;
#ifdef CL_VERSION_1_2
    cl_mem srcImage = clCreateImage(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                      &image_format, &image_desc_src, (void *) srcData, NULL);

    cl_mem fourier2dImageA = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                             &image_format, &image_desc_fourier2d, NULL, &errNo);

    cl_mem fourier2dImageB = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                             &image_format, &image_desc_fourier2d, NULL, NULL);

    cl_mem sliceImage = clCreateImage(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                &image_format, &image_desc_slices, NULL, NULL);
#else
    cl_mem srcImage = clCreateImage3D(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                      &image_format, width, height, depth,
                                      rowPitchSrc, slicePitchSrc, (void *) srcData, NULL);

    cl_mem fourier2dImageA = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                             &image_format, paddedWidth, paddedWidth, height, 0, 0, NULL, NULL);

    cl_mem fourier2dImageB = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                             &image_format, paddedWidth, paddedWidth, height, 0, 0, NULL, NULL);

    cl_mem sliceImage = clCreateImage3D(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                  &image_format, width, width, height, 0, 0, NULL, NULL);
#endif

    cl_mem casBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, NULL, NULL);

    cl_mem cosBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, sizeof(float) * depth, NULL, &errNo);
    cl_mem sinBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, sizeof(float) * depth, NULL, &errNo);

    float twoPiN = (2 * CV_PI) / paddedWidth;

    clSetKernelArg(_calcCasKernel, 0, sizeof(cl_mem), (void *) &casBuf);
    clSetKernelArg(_calcCasKernel, 1, sizeof(size_t), (void *) &paddedWidth);
    clSetKernelArg(_calcCasKernel, 2, sizeof(float), (void *) &twoPiN);

    float coeffTrig = depth / 360.0;

    clSetKernelArg(_calcTrigKernel, 0, sizeof(cl_mem), (void *) &cosBuf);
    clSetKernelArg(_calcTrigKernel, 1, sizeof(cl_mem), (void *) &sinBuf);
    clSetKernelArg(_calcTrigKernel, 2, sizeof(float), (void *) &coeffTrig);
    
    size_t globalThreadsCalcCas[2] = {paddedWidth, paddedWidth};
    clEnqueueNDRangeKernel(_queue, _calcCasKernel, 2, NULL, globalThreadsCalcCas, NULL, 0, NULL, eventList);
    clEnqueueNDRangeKernel(_queue, _calcTrigKernel, 2, NULL, globalThreadsCalcCas, NULL, 0, NULL, eventList + 1);

    clSetKernelArg(_fourier2dKernel, 0, sizeof(cl_mem), (void *) &srcImage);
    clSetKernelArg(_fourier2dKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
    clSetKernelArg(_fourier2dKernel, 2, sizeof(cl_mem), (void *) &casBuf);
    clSetKernelArg(_fourier2dKernel, 3, sizeof(cl_mem), (void *) &cosBuf);
    clSetKernelArg(_fourier2dKernel, 4, sizeof(cl_mem), (void *) &sinBuf);

    size_t globalThreadsFourier2d[3] = {width, height, depth};
    clEnqueueNDRangeKernel(_queue, _fourier2dKernel, 3, NULL, globalThreadsFourier2d, NULL, 2, eventList, eventList + 2);

    const float coeff = 1.0 / paddedWidth;

    clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
    clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageB);
    clSetKernelArg(_dht1dTransposeKernel, 2, sizeof(cl_mem), (void *) &casBuf);
    clSetKernelArg(_dht1dTransposeKernel, 3, sizeof(cl_mem), (void *) &coeff);

    size_t globalThreadsDht1dTranspose[3] = {paddedWidth, paddedWidth, height};
    clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, NULL, globalThreadsDht1dTranspose,
                                       NULL, 1, eventList + 2, eventList + 3);

    clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageB);
    clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
    clSetKernelArg(_dht1dTransposeKernel, 2, sizeof(cl_mem), (void *) &casBuf);
    clSetKernelArg(_dht1dTransposeKernel, 3, sizeof(cl_mem), (void *) &coeff);

    clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, NULL, globalThreadsDht1dTranspose,
                                       NULL, 1, eventList + 3, eventList + 4);

    clSetKernelArg(_butterflyDht2dKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
    clSetKernelArg(_butterflyDht2dKernel, 1, sizeof(cl_mem), (void *) &sliceImage);

    size_t globalThreadsButterfly[3] = {paddedWidth / 2, paddedWidth / 2, height};
    clEnqueueNDRangeKernel(_queue, _butterflyDht2dKernel, 3, NULL, globalThreadsButterfly,
                                       NULL, 1, eventList + 4, eventList + 5);

    uchar * sliceData = (uchar *) clEnqueueMapImage(_queue, sliceImage,
                                             CL_TRUE, CL_MEM_WRITE_ONLY, origin, regionSlice,
                                             &rowPitchDst, &slicePitchDst,
                                             1, eventList + 5, eventList + 6, &errNo);

    clWaitForEvents(1, eventList + 6);

    qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime << sliceData;

    _slicesOCL.resize(height);

    cv::Mat helperMat;
    cv::Mat mask;

    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;

    for (size_t i = 0; i != _slicesOCL.size(); ++ i) {
        helperMat = cv::Mat((int) width, (int) width, CV_32FC1, (void *) (sliceData + i * slicePitchDst));

        minMaxLoc(helperMat, &minVal, &maxVal, &minLoc, &maxLoc);

        qDebug() << minVal << maxVal;
        cv::convertScaleAbs(helperMat, helperMat, 256.0 / (maxVal - minVal), - minVal / (maxVal - minVal));
        //cv::threshold(helperMat, _slicesOCL.at(i), 60, 255, CV_THRESH_BINARY);
        //cv::GaussianBlur(helperMat, helperMat, cv::Size(5, 5), 1.4);
        //cv::erode(helperMat, helperMat, cv::Mat(5, 5, CV_8UC1));

        cv::threshold(helperMat, mask, 60, 255, CV_THRESH_BINARY);
        cv::bitwise_and(helperMat, helperMat, _slicesOCL.at(i), mask);

        //cv::GaussianBlur(_slicesOCL.at(i), _slicesOCL.at(i), cv::Size(3, 3), 1.4);

        //cv::dilate(_slicesOCL.at(i), _slicesOCL.at(i), cv::Mat(3, 3, CV_8UC1));
        //helperMat.convertTo(_slicesOCL.at(i), CV_8UC1, 256.0);
    }

    showSliceWithNumber(0);

    delete [] srcData;

    for (int i = 0; i != 7; ++ i) {
        clReleaseEvent(eventList[i]);
    }

    clReleaseMemObject(srcImage);
    clReleaseMemObject(fourier2dImageA);
    clReleaseMemObject(fourier2dImageB);

    clEnqueueUnmapMemObject(_queue, sliceImage, sliceData, 0, NULL, NULL);
    clFinish(_queue);

    clReleaseMemObject(sliceImage);
    clReleaseMemObject(casBuf);
    clReleaseMemObject(cosBuf);
    clReleaseMemObject(sinBuf);

    SliceInfo::SliceSettings sliceSettings;

    cv::Mat slice(_slicesOCL.at(0));

    size_t oneSliceSize = slice.elemSize() * slice.total();
    size_t sliceCount = _slicesOCL.size();

    uchar * mergedData = new uchar[oneSliceSize * sliceCount];

    for (size_t i = 0; i != sliceCount; ++ i) {
        memcpy(mergedData + i * oneSliceSize, _slicesOCL.at(i).data, oneSliceSize);
    }

    sliceSettings.mergedData = QSharedPointer<uchar>(mergedData);

    sliceSettings.alignment = (slice.step & 3) ? 1 : 4;
    sliceSettings.rowLength = slice.step1();

    sliceSettings.size = {(size_t) slice.cols, (size_t) slice.rows, sliceCount};
    sliceSettings.scaling = {
        sliceSettings.size[0] / (float) sliceSettings.size[1] / SCALE_COEFF,
        sliceSettings.size[1] / (float) sliceSettings.size[1] / SCALE_COEFF,
        sliceSettings.size[2] / (float) sliceSettings.size[1] / SCALE_COEFF,
    };

    sliceSettings.sliceDataType = SliceInfo::Int8;

    emit slicesProcessed(sliceSettings);
}

void Reconstructor::resetV(std::vector<cv::Mat *> & vec, const int & newSize) {
    for (std::vector<cv::Mat *>::iterator it = vec.begin(); it != vec.end(); ++ it) {
        delete *it;
    }
    vec.clear();
    vec.resize(newSize);
}

void Reconstructor::reset(const int & newSize) {
    resetV(_slices, newSize);
}

void Reconstructor::readFiles(QStringList fileNames) {
    reset();

    QStringListIterator it(fileNames);

    cv::Size imSize = cv::Size();
    int i = 0;

    while (it.hasNext()) {
       _src.push_back(cv::imread(it.next().toStdString(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE));
       _src.at(i).convertTo(_src.at(i), CV_32FC1, 1 / 256.0);

       // all images must be the same size or resize
       if (imSize != cv::Size(0, 0)) {
           if (_src.at(i).size() != imSize) {
               cv::resize(_src.at(i), _src.at(i), imSize);
           }
       }
       else {
           imSize = _src.at(i).size();
       }

       i++;
    }

    //ReconstructionData reconstructionData;

    //reconstructionData.src = &_src;
    //reconstructionData.slices = &_slice;

    //reconstructionData.sliceCount = imSize.height;

    reconstruct();

    //cv::parallel_for_(cv::Range(0, reconstructionData.sliceCount), Reconstructor(&reconstructionData));

    cv::namedWindow(SLICES_IMAGE_WINDOW);
    cv::namedWindow(SLICE_POSITION);

    showSliceWithNumber(0);
}

void Reconstructor::changeSliceNumber(const int & ds) {
    if (_slicesOCL.size()) {
        _sliceNumber += ds;
        _sliceNumber %= _slicesOCL.size();
        showSliceWithNumber(_sliceNumber);
    }
}

void Reconstructor::showSliceWithNumber(const int & sliceNumber) {
    cv::imshow(SLICES_IMAGE_WINDOW, _slicesOCL.at(sliceNumber));

    cv::Mat slicePosition(_src.at(sliceNumber % _src.size()));
    cv::cvtColor(slicePosition, slicePosition, CV_GRAY2BGR);
    cv::rectangle(slicePosition, cv::Rect(0, sliceNumber, slicePosition.cols, 1), cv::Scalar(0, 0, 255));
    cv::imshow(SLICE_POSITION, slicePosition);
}

