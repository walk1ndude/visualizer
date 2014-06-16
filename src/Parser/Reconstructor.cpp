#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Parser/Reconstructor.h"

#define SLICES_IMAGE_WINDOW "slices"
#define SLICE_POSITION "position"

#define toRad(x) ((x) * CV_PI / 180.0)
#define PADDED_INCREASE 1

#define SIGMA_GAUSS 1.3
#define KERN_SIZE_GAUSS 3

#define SCALE_COEFF ((float) 0.7)

namespace Parser {
    Reconstructor::Reconstructor(QObject * parent) :
        QObject(parent),
        _sliceNumber(0) {

        initOpenCL();
    }

    Reconstructor::~Reconstructor() {
        reset();

        clReleaseKernel(_calcTablesKernel);
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

        _gauss1dKernel = clCreateKernel(_programSlice, "gauss1d", NULL);
        _calcTablesKernel = clCreateKernel(_programSlice, "calcTables", NULL);
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

        float r;
        float s = 2.0 * SIGMA_GAUSS * SIGMA_GAUSS;

        float sum = 0.0;
        float gaussTab[KERN_SIZE_GAUSS];

        for (int x = - KERN_SIZE_GAUSS / 2; x <= KERN_SIZE_GAUSS / 2 + 1; ++ x) {
            r = x * x;
            gaussTab[x + 2] = (exp(-r / s)) / (CV_PI * s);
            sum += gaussTab[x + 2];
        }

        // normalize the Kernel
        for(int i = 0; i < KERN_SIZE_GAUSS; ++i) {
            gaussTab[i] /= sum;
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

        cl_image_desc image_desc_gauss;
        image_desc_gauss.image_type = CL_MEM_OBJECT_IMAGE3D;
        image_desc_gauss.image_width = width;
        image_desc_gauss.image_height = height;
        image_desc_gauss.image_depth = depth;
        image_desc_gauss.buffer = NULL;
        image_desc_gauss.num_mip_levels = 0;
        image_desc_gauss.image_row_pitch = 0;
        image_desc_gauss.image_slice_pitch = 0;
        image_desc_gauss.num_samples = 0;

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

        cl_event eventList[6];

        int errNo;

    #ifdef CL_VERSION_1_2
        cl_mem srcImage = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, &image_desc_src, (void *) srcData, NULL);

        cl_mem gaussImage = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                          &image_format, &image_desc_gauss, NULL, NULL);

    #else
        cl_mem srcImage = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, width, height, depth,
                                          rowPitchSrc, slicePitchSrc, (void *) srcData, NULL);

        cl_mem gaussImage = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, width, height, depth,
                                          NULL, NULL, NULL, NULL);

    #endif

        uint dir0 = 0;
        uint dir1 = 1;

        uint kernGaussSize = KERN_SIZE_GAUSS / 2;

        cl_mem gaussBuf = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * KERN_SIZE_GAUSS, (void *) &gaussTab, NULL);

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 2, sizeof(cl_mem), (void *) &gaussBuf);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir1);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 6, sizeof(uint), (void *) &kernGaussSize);

        size_t globalThreadsGauss1d[3] = {width, height, depth};
        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, NULL, globalThreadsGauss1d, NULL, 0, NULL, eventList);

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 2, sizeof(cl_mem), (void *) &gaussBuf);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir1);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 6, sizeof(uint), (void *) &kernGaussSize);

        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, NULL, globalThreadsGauss1d, NULL, 1, eventList, eventList + 1);

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 2, sizeof(cl_mem), (void *) &gaussBuf);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir1);
        clSetKernelArg(_gauss1dKernel, 6, sizeof(uint), (void *) &kernGaussSize);

        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, NULL, globalThreadsGauss1d, NULL, 1, eventList + 1, eventList + 2);

        clWaitForEvents(1, eventList + 2);

        delete [] srcData;

        clReleaseMemObject(srcImage);
        clReleaseMemObject(gaussBuf);

        for (int i = 0; i != 3; ++ i) {
            clReleaseEvent(eventList[i]);
        }

    #ifdef CL_VERSION_1_2
        cl_mem fourier2dImageA = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                                 &image_format, &image_desc_fourier2d, NULL, &errNo);
    #else
        cl_mem fourier2dImageA = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                                 &image_format, paddedWidth, paddedWidth, height, 0, 0, NULL, NULL);
    #endif

        cl_mem casBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, NULL, NULL);
        cl_mem tanBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, NULL, NULL);
        cl_mem radBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, NULL, NULL);

        float twoPiN = (2 * CV_PI) / paddedWidth;

        clSetKernelArg(_calcTablesKernel, 0, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_calcTablesKernel, 1, sizeof(cl_mem), (void *) &tanBuf);
        clSetKernelArg(_calcTablesKernel, 2, sizeof(cl_mem), (void *) &radBuf);
        clSetKernelArg(_calcTablesKernel, 3, sizeof(size_t), (void *) &paddedWidth);
        clSetKernelArg(_calcTablesKernel, 4, sizeof(size_t), (void *) &paddedWidth);
        clSetKernelArg(_calcTablesKernel, 5, sizeof(float), (void *) &twoPiN);

        size_t globalThreadsCalcTables[2] = {paddedWidth, paddedWidth};
        clEnqueueNDRangeKernel(_queue, _calcTablesKernel, 2, NULL, globalThreadsCalcTables, NULL, 0, NULL, eventList);

        clSetKernelArg(_fourier2dKernel, 0, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_fourier2dKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_fourier2dKernel, 2, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_fourier2dKernel, 3, sizeof(cl_mem), (void *) &tanBuf);
        clSetKernelArg(_fourier2dKernel, 4, sizeof(cl_mem), (void *) &radBuf);

        size_t globalThreadsFourier2d[3] = {paddedWidth, paddedWidth, height};
        clEnqueueNDRangeKernel(_queue, _fourier2dKernel, 3, NULL, globalThreadsFourier2d, NULL, 1, eventList, eventList + 1);

        clWaitForEvents(1, eventList + 1);

        clReleaseMemObject(gaussImage);

    #ifdef CL_VERSION_1_2
        cl_mem fourier2dImageB = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                               &image_format, &image_desc_fourier2d, NULL, NULL);
    #else
        cl_mem fourier2dImageB = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                                 &image_format, paddedWidth, paddedWidth, height, 0, 0, NULL, NULL);
    #endif

        const float coeff = 1.0 / paddedWidth;

        clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageB);
        clSetKernelArg(_dht1dTransposeKernel, 2, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_dht1dTransposeKernel, 3, sizeof(cl_mem), (void *) &coeff);

        size_t globalThreadsDht1dTranspose[3] = {paddedWidth, paddedWidth, height};
        clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, NULL, globalThreadsDht1dTranspose,
                                           NULL, 1, eventList + 1, eventList + 2);

        clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageB);
        clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_dht1dTransposeKernel, 2, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_dht1dTransposeKernel, 3, sizeof(cl_mem), (void *) &coeff);

        clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, NULL, globalThreadsDht1dTranspose,
                                           NULL, 1, eventList + 2, eventList + 3);

        clWaitForEvents(1, eventList + 3);

        clReleaseMemObject(fourier2dImageB);
        clReleaseMemObject(casBuf);
        clReleaseMemObject(tanBuf);
        clReleaseMemObject(radBuf);

    #ifdef CL_VERSION_1_2
        cl_mem sliceImage = clCreateImage(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                          &image_format, &image_desc_slices, NULL, NULL);
    #else
        cl_mem sliceImage = clCreateImage3D(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                            &image_format, width, width, height, 0, 0, NULL, NULL);
    #endif

        clSetKernelArg(_butterflyDht2dKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_butterflyDht2dKernel, 1, sizeof(cl_mem), (void *) &sliceImage);

        size_t globalThreadsButterfly[3] = {paddedWidth / 2, paddedWidth / 2, height};
        clEnqueueNDRangeKernel(_queue, _butterflyDht2dKernel, 3, NULL, globalThreadsButterfly,
                                           NULL, 1, eventList + 3, eventList + 4);

        uchar * sliceData = (uchar *) clEnqueueMapImage(_queue, sliceImage,
                                                 CL_TRUE, CL_MEM_WRITE_ONLY, origin, regionSlice,
                                                 &rowPitchDst, &slicePitchDst,
                                                 1, eventList + 4, eventList + 5, NULL);


        clWaitForEvents(1, eventList + 5);

        qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime << sliceData;

        _slicesOCL.resize(height);

        cv::Mat helperMat;
        cv::Mat mask;

        double minVal;
        double maxVal;

        double minValVolume;
        double maxValVolume;

        helperMat = cv::Mat((int) width, (int) width, CV_32FC1, (void *) (sliceData + slicePitchDst));
        cv::minMaxLoc(helperMat, &minValVolume, &maxValVolume, NULL, NULL);
        helperMat.copyTo(_slicesOCL.at(0));

        for (size_t i = 1; i != _slicesOCL.size(); ++ i) {
            helperMat = cv::Mat((int) width, (int) width, CV_32FC1, (void *) (sliceData + i * slicePitchDst));

            cv::minMaxLoc(helperMat, &minVal, &maxVal, NULL, NULL);

            helperMat.copyTo(_slicesOCL.at(i));

            maxValVolume = std::max(maxValVolume, maxVal);
            minValVolume = std::min(minValVolume, minVal);
        }

        for (size_t i = 0; i != _slicesOCL.size(); ++ i) {
            cv::convertScaleAbs(_slicesOCL.at(i),
                                _slicesOCL.at(i),
                                4 * 256.0 / (maxValVolume - minValVolume),
                                minValVolume / (minValVolume - maxValVolume));

            //cv::adaptiveThreshold(_slicesOCL.at(i), mask, 200, cv::ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 5, 1.2);
            cv::threshold(_slicesOCL.at(i), mask, 150, 255, CV_THRESH_BINARY);


            helperMat = cv::Scalar(0);
            cv::bitwise_and(_slicesOCL.at(i), _slicesOCL.at(i), helperMat, mask);
            helperMat.copyTo(_slicesOCL.at(i));
        }

        showSliceWithNumber(0);

        for (int i = 0; i != 6; ++ i) {
            clReleaseEvent(eventList[i]);
        }

        clReleaseMemObject(fourier2dImageA);

        clEnqueueUnmapMemObject(_queue, sliceImage, sliceData, 0, NULL, NULL);
        clFinish(_queue);

        clReleaseMemObject(sliceImage);

        SliceInfo::Slices slices;

        cv::Mat slice(_slicesOCL.at(0));

        size_t oneSliceSize = slice.elemSize() * slice.total();
        size_t sliceCount = _slicesOCL.size();

        uchar * mergedData = new uchar[oneSliceSize * sliceCount];

        for (size_t i = 0; i != sliceCount; ++ i) {
            memcpy(mergedData + i * oneSliceSize, _slicesOCL.at(i).data, oneSliceSize);
        }

        slices.texture.mergedData = TextureInfo::MergedDataPointer(mergedData);

        slices.texture.pixelTransferOptions.setAlignment((slice.step & 3) ? 1 : 4);
        slices.texture.pixelTransferOptions.setRowLength((int) slice.step1());

        slices.texture.size.setX(slice.cols);
        slices.texture.size.setX(slice.rows);
        slices.texture.size.setX(sliceCount);

        slices.texture.scaling = {
            slices.texture.size.x() / slices.texture.size.y() / SCALE_COEFF,
            slices.texture.size.y() / slices.texture.size.y() / SCALE_COEFF,
            slices.texture.size.z() / slices.texture.size.y() / SCALE_COEFF,
        };

        slices.texture.pixelType = QOpenGLTexture::UInt8;
        slices.texture.textureFormat = QOpenGLTexture::R8_UNorm;
        slices.texture.pixelFormat = QOpenGLTexture::Red;
        slices.texture.target = QOpenGLTexture::Target3D;

        emit slicesProcessed(slices);
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

    void Reconstructor::readFiles(const QStringList & fileNames) {
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
}
