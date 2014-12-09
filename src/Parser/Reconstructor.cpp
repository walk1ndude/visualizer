#include "Parser/Reconstructor.h"
#include "Parser/parallelprocessing.hpp"

#define SLICES_IMAGE_WINDOW "slices"
#define SLICE_POSITION "position"

#define GAUSS
// opencl settings for Iris 5200
#define IRIS_5200
//#define AMD_BARTS

#ifdef IRIS_5200
#define WORK_GROUP_WIDTH 8
#define WORK_GROUP_HEIGHT 8
#define WORK_GROUP_DEPTH 8

#define PADDED_INCREASE 1.0f
#endif

#ifdef AMD_BARTS
#define WORK_GROUP_WIDTH 4
#define WORK_GROUP_HEIGHT 4
#define WORK_GROUP_DEPTH 4

#define CL_CONTEXT_OFFLINE_DEVICES_AMD 0x403F

#define PADDED_INCREASE 1.5f
#endif

#define SIGMA_GAUSS 1.5
#define KERN_SIZE_GAUSS 5

#define ALL_EVENTS 9

#define GAUSS_1D_COMPLETED_EVENT_X 0
#define GAUSS_1D_COMPLETED_EVENT_Y 1
#define GAUSS_1D_COMPLETED_EVENT_Z 2
#define CALC_TABLES_COMPLETED_EVENT 3

#define DHT_1D_TO_2D_COMPLETED_EVENT 4
#define DHT_2D_I_FIRST_1D_COMPLETED_EVENT 5
#define DHT_2D_I_SECOND_1D_COMPLETED_EVENT 6
#define DHT_2D_I_COMPLETED_EVENT 7
#define MAP_FINAL_COMPLETED_EVENT 8

#define ALL_EVENTS_COMPLETED 8

namespace Parser {
    Reconstructor::Reconstructor() :
        AbstractParser(),
        _sliceNumber(0),
        _isOCLInitialized(false) {
    }

    Reconstructor::~Reconstructor() {
        reset();

        if (_isOCLInitialized) {
            releaseOCLResources();
        }
    }

    void Reconstructor::reset() {
        qDeleteAll(_slicesOCL.begin(), _slicesOCL.end());
    }

    void Reconstructor::initOCL() {
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

        cl_int errNo;

        cl_platform_id * platforms;
        cl_uint platforms_n;

        clGetPlatformIDs(0, nullptr, &platforms_n);
        platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * (platforms_n + 1));
        clGetPlatformIDs(platforms_n, platforms, &platforms_n);

        qDebug() << clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &_device_id, nullptr);

#ifdef AMD_BARTS
        _context = clCreateContext(0, 1, &_device_id, nullptr, nullptr, &errNo);
#else
        _context = clCreateContextFromType(nullptr, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &errNo);
#endif
        qDebug() << errNo;

        size_t size[3];
        clGetDeviceInfo(_device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, size, nullptr);
        
        _queue = clCreateCommandQueue(_context, _device_id, 0, nullptr);
        
        _programReconstruction = clCreateProgramWithSource(_context, 1, (const char **) &programText,
                                                  (const size_t *) &programLength, nullptr);

        free(programText);
        free(platforms);

        qDebug() << "Building OpenCL Program, error: " << clBuildProgram(_programReconstruction, 1, &_device_id, nullptr, nullptr, nullptr);

        _gauss1dKernel = clCreateKernel(_programReconstruction, "gauss1d", nullptr);
        _calcTablesKernel = clCreateKernel(_programReconstruction, "calcTables", nullptr);
        _fourier2dKernel = clCreateKernel(_programReconstruction, "fourier2d", nullptr);
        _dht1dTransposeKernel = clCreateKernel(_programReconstruction, "dht1dTranspose", nullptr);
        _butterflyDht2dKernel = clCreateKernel(_programReconstruction, "butterflyDht2d", nullptr);
        
        _isOCLInitialized = true;
    }
    
    void Reconstructor::releaseOCLResources() {
        clReleaseKernel(_calcTablesKernel);
        clReleaseKernel(_butterflyDht2dKernel);
        clReleaseKernel(_fourier2dKernel);
        clReleaseKernel(_dht1dTransposeKernel);
        
        clReleaseProgram(_programReconstruction);
        clReleaseCommandQueue(_queue);
#ifdef CL_VERSION_1_2
        clReleaseDevice(_device_id);
#endif
        clReleaseContext(_context);
    }

    void Reconstructor::reconstruct() {
        if (!_isOCLInitialized) {
            initOCL();
        }
        
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
        uchar * posSlice = srcData;

        for (const cv::Mat & slice : _src) {
            memcpy(posSlice, slice.data, slicePitchSrc);
            posSlice += slicePitchSrc;
        }

        float gaussTab[KERN_SIZE_GAUSS];

#ifdef GAUSS
        float r;
        float s = 2.0 * SIGMA_GAUSS * SIGMA_GAUSS;
        
        float sum = 0.0;
        
        for (int x = - KERN_SIZE_GAUSS / 2; x <= KERN_SIZE_GAUSS / 2 + 1; ++ x) {
            r = x * x;
            gaussTab[x + 2] = (exp(-r / s)) / (CV_PI * s);
            sum += gaussTab[x + 2];
        }

        // normalize the Kernel
        for(int i = 0; i < KERN_SIZE_GAUSS; ++i) {
            gaussTab[i] /= sum;
        }
#else
        for(int i = 0; i < KERN_SIZE_GAUSS; ++i) {
            gaussTab[i] = KERN_SIZE_GAUSS;
        }
#endif

        cl_image_format image_format;
        image_format.image_channel_data_type = CL_FLOAT;
        image_format.image_channel_order = CL_R;

    #ifdef CL_VERSION_1_2
        cl_image_desc image_desc_src;
        image_desc_src.image_type = CL_MEM_OBJECT_IMAGE3D;
        image_desc_src.image_width = width;
        image_desc_src.image_height = height;
        image_desc_src.image_depth = depth;
        image_desc_src.buffer = nullptr;
        image_desc_src.num_mip_levels = 0;
        image_desc_src.image_row_pitch = rowPitchSrc;
        image_desc_src.image_slice_pitch = slicePitchSrc;
        image_desc_src.num_samples = 0;

        cl_image_desc image_desc_gauss;
        image_desc_gauss.image_type = CL_MEM_OBJECT_IMAGE3D;
        image_desc_gauss.image_width = width;
        image_desc_gauss.image_height = height;
        image_desc_gauss.image_depth = depth;
        image_desc_gauss.buffer = nullptr;
        image_desc_gauss.num_mip_levels = 0;
        image_desc_gauss.image_row_pitch = rowPitchSrc;
        image_desc_gauss.image_slice_pitch = slicePitchSrc;
        image_desc_gauss.num_samples = 0;

        cl_image_desc image_desc_fourier2d;
        image_desc_fourier2d.image_type = CL_MEM_OBJECT_IMAGE3D;
        image_desc_fourier2d.image_width = paddedWidth;
        image_desc_fourier2d.image_height = paddedWidth;
        image_desc_fourier2d.image_depth = height;
        image_desc_fourier2d.buffer = nullptr;
        image_desc_fourier2d.num_mip_levels = 0;
        image_desc_fourier2d.image_row_pitch = 0;
        image_desc_fourier2d.image_slice_pitch = 0;
        image_desc_fourier2d.num_samples = 0;

        cl_image_desc image_desc_slices;
        image_desc_slices.image_type = CL_MEM_OBJECT_IMAGE3D;
        image_desc_slices.image_width = width;
        image_desc_slices.image_height = width;
        image_desc_slices.image_depth = height;
        image_desc_slices.buffer = nullptr;
        image_desc_slices.num_mip_levels = 0;
        image_desc_slices.image_row_pitch = 0;
        image_desc_slices.image_slice_pitch = 0;
        image_desc_slices.num_samples = 0;
    #endif
        size_t origin[3] = {0, 0, 0};
        size_t regionSlice[3] = {width, width, height};

        cl_event eventList[ALL_EVENTS];

        cl_int errNo;

    #ifdef CL_VERSION_1_2
        cl_mem srcImage = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, &image_desc_src, (void *) srcData, &errNo);

        cl_mem gaussImage = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                          &image_format, &image_desc_gauss, nullptr, &errNo);

    #else
        cl_mem srcImage = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, width, height, depth,
                                          rowPitchSrc, slicePitchSrc, (void *) srcData, nullptr);

        cl_mem gaussImage = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                          &image_format, width, height, depth,
                                          0, 0, nullptr, nullptr);

    #endif

        uint dir0 = 0;
        uint dir1 = 1;

        int kernGaussSize = KERN_SIZE_GAUSS / 2;

        cl_mem gaussBuf = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                         sizeof(float) * KERN_SIZE_GAUSS, (void *) &gaussTab, nullptr);
        
        size_t globalThreadsGauss1d[3] = {width, height, depth};
        size_t localThreadsGauss1d[3] = {WORK_GROUP_WIDTH, WORK_GROUP_HEIGHT, 6};

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 2, sizeof(cl_mem), (void *) &gaussBuf);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir1);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 6, sizeof(int), (void *) &kernGaussSize);
        
        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, nullptr, globalThreadsGauss1d, localThreadsGauss1d,
                               0, nullptr, eventList + GAUSS_1D_COMPLETED_EVENT_X);
        
        clWaitForEvents(GAUSS_1D_COMPLETED_EVENT_X, eventList);

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir1);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir0);
        
        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, nullptr, globalThreadsGauss1d, localThreadsGauss1d,
                               GAUSS_1D_COMPLETED_EVENT_Y, eventList, eventList + GAUSS_1D_COMPLETED_EVENT_Y);
        
        clWaitForEvents(GAUSS_1D_COMPLETED_EVENT_Y, eventList);

        clSetKernelArg(_gauss1dKernel, 0, sizeof(cl_mem), (void *) &srcImage);
        clSetKernelArg(_gauss1dKernel, 1, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_gauss1dKernel, 3, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 4, sizeof(uint), (void *) &dir0);
        clSetKernelArg(_gauss1dKernel, 5, sizeof(uint), (void *) &dir1);

        clEnqueueNDRangeKernel(_queue, _gauss1dKernel, 3, nullptr, globalThreadsGauss1d, localThreadsGauss1d,
                               GAUSS_1D_COMPLETED_EVENT_Z, eventList, eventList + GAUSS_1D_COMPLETED_EVENT_Z);
        
        clWaitForEvents(GAUSS_1D_COMPLETED_EVENT_Z, eventList);
        
        delete [] srcData;
        clReleaseMemObject(srcImage);
        clReleaseMemObject(gaussBuf);
        
        cl_mem casBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, nullptr, nullptr);
        cl_mem tanBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, nullptr, nullptr);
        cl_mem radBuf = clCreateBuffer(_context, CL_MEM_READ_WRITE, slicePitchFourier2d, nullptr, nullptr);

        float twoPiN = (2 * CV_PI) / paddedWidth;

        int paddedWidthKernelArg = (int) paddedWidth;

        clSetKernelArg(_calcTablesKernel, 0, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_calcTablesKernel, 1, sizeof(cl_mem), (void *) &tanBuf);
        clSetKernelArg(_calcTablesKernel, 2, sizeof(cl_mem), (void *) &radBuf);
        clSetKernelArg(_calcTablesKernel, 3, sizeof(int), (void *) &paddedWidthKernelArg);
        clSetKernelArg(_calcTablesKernel, 4, sizeof(int), (void *) &paddedWidthKernelArg);
        clSetKernelArg(_calcTablesKernel, 5, sizeof(float), (void *) &twoPiN);
        
        size_t globalThreadsCalcTables[2] = {paddedWidth, paddedWidth};
        size_t localThreadsCalcTables[2] = {WORK_GROUP_WIDTH, WORK_GROUP_WIDTH};
        
        clEnqueueNDRangeKernel(_queue, _calcTablesKernel, 2, nullptr,
                               globalThreadsCalcTables, localThreadsCalcTables, 0, nullptr,
                               eventList + CALC_TABLES_COMPLETED_EVENT);
        
        clWaitForEvents(CALC_TABLES_COMPLETED_EVENT, eventList);
        
        
#ifdef CL_VERSION_1_2
        cl_mem fourier2dImageA = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                               &image_format, &image_desc_fourier2d, nullptr, &errNo);
#else
        cl_mem fourier2dImageA = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                                 &image_format, paddedWidth, paddedWidth, height, 0, 0, nullptr, nullptr);
#endif
        
        clSetKernelArg(_fourier2dKernel, 0, sizeof(cl_mem), (void *) &gaussImage);
        clSetKernelArg(_fourier2dKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_fourier2dKernel, 2, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_fourier2dKernel, 3, sizeof(cl_mem), (void *) &tanBuf);
        clSetKernelArg(_fourier2dKernel, 4, sizeof(cl_mem), (void *) &radBuf);
        
        size_t globalThreadsFourier2d[3] = {paddedWidth, paddedWidth, height};
        size_t localThreadsFourier2d[3] = {WORK_GROUP_WIDTH, WORK_GROUP_HEIGHT, WORK_GROUP_DEPTH};
        
        clEnqueueNDRangeKernel(_queue, _fourier2dKernel, 3, nullptr, globalThreadsFourier2d, localThreadsFourier2d,
                               CALC_TABLES_COMPLETED_EVENT, eventList, eventList + DHT_1D_TO_2D_COMPLETED_EVENT);

        clWaitForEvents(DHT_1D_TO_2D_COMPLETED_EVENT, eventList);

        clReleaseMemObject(gaussImage);

    #ifdef CL_VERSION_1_2
        cl_mem fourier2dImageB = clCreateImage(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                               &image_format, &image_desc_fourier2d, nullptr, &errNo);
    #else
        cl_mem fourier2dImageB = clCreateImage3D(_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                                                 &image_format, paddedWidth, paddedWidth, height, 0, 0, nullptr, nullptr);
    #endif

        float coeff = 1.0f / paddedWidth;
        size_t globalThreadsDht1dTranspose[3] = {paddedWidth, paddedWidth, height};
        size_t localThreadsDht1dTranspose[3] = {WORK_GROUP_WIDTH, WORK_GROUP_HEIGHT, WORK_GROUP_DEPTH};
        
        clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageB);
        clSetKernelArg(_dht1dTransposeKernel, 2, sizeof(cl_mem), (void *) &casBuf);
        clSetKernelArg(_dht1dTransposeKernel, 3, sizeof(float), (void *) &coeff);
        
        clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, nullptr, globalThreadsDht1dTranspose,
                                          localThreadsDht1dTranspose, DHT_1D_TO_2D_COMPLETED_EVENT, eventList, eventList + DHT_2D_I_FIRST_1D_COMPLETED_EVENT);

        clWaitForEvents(DHT_2D_I_FIRST_1D_COMPLETED_EVENT, eventList);
        
        clSetKernelArg(_dht1dTransposeKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageB);
        clSetKernelArg(_dht1dTransposeKernel, 1, sizeof(cl_mem), (void *) &fourier2dImageA);
        
        clEnqueueNDRangeKernel(_queue, _dht1dTransposeKernel, 3, nullptr, globalThreadsDht1dTranspose,
                               localThreadsDht1dTranspose, DHT_2D_I_FIRST_1D_COMPLETED_EVENT, eventList, eventList + DHT_2D_I_SECOND_1D_COMPLETED_EVENT);

        clWaitForEvents(DHT_2D_I_SECOND_1D_COMPLETED_EVENT, eventList);
        
        clReleaseMemObject(fourier2dImageB);
        
#ifdef CL_VERSION_1_2
        cl_mem sliceImage = clCreateImage(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                          &image_format, &image_desc_slices, nullptr, &errNo);
#else
        cl_mem sliceImage = clCreateImage3D(_context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                                            &image_format, width, width, height, 0, 0, nullptr, nullptr);
#endif

        size_t globalThreadsButterfly[3] = {paddedWidth / 2 + 1, paddedWidth / 2 + 1, height};

        clSetKernelArg(_butterflyDht2dKernel, 0, sizeof(cl_mem), (void *) &fourier2dImageA);
        clSetKernelArg(_butterflyDht2dKernel, 1, sizeof(cl_mem), (void *) &sliceImage);
        
        clEnqueueNDRangeKernel(_queue, _butterflyDht2dKernel, 3, nullptr, globalThreadsButterfly,
                               nullptr, DHT_2D_I_SECOND_1D_COMPLETED_EVENT, eventList, eventList + DHT_2D_I_COMPLETED_EVENT);
        
        uchar * sliceData = (uchar *) clEnqueueMapImage(_queue, sliceImage,
                                                 CL_TRUE, CL_MEM_WRITE_ONLY, origin, regionSlice,
                                                 &rowPitchDst, &slicePitchDst,
                                                 DHT_2D_I_COMPLETED_EVENT, eventList, eventList + MAP_FINAL_COMPLETED_EVENT, &errNo);
        
        qDebug() << errNo;
        
        clWaitForEvents(ALL_EVENTS_COMPLETED, eventList);
        
        cv::Mat helperMat;
        
        double minVal;
        double maxVal;
        
        // there will be never such values, so it's save
        double minValVolume = 1000.0f;
        double maxValVolume = -1000.0f;
        
        _slicesOCL.resize((int) height);
        posSlice = sliceData;
        QMutableVectorIterator<cv::Mat *>it(_slicesOCL);
        
        qDebug() << "Elapsed Time: " << cv::getTickCount() / cv::getTickFrequency() - startTime << sliceData;
        
        while (it.hasNext()) {
            helperMat = cv::Mat((int) width, (int) width, CV_32FC1, (void *) (posSlice));
            posSlice += slicePitchDst;
            
            cv::minMaxLoc(helperMat, &minVal, &maxVal);
            
            it.next() = new cv::Mat(helperMat);
            
            maxValVolume = std::max(maxValVolume, maxVal);
            minValVolume = std::min(minValVolume, minVal);
        }
        
        cv::Mat result;
        for (cv::Mat * slice : _slicesOCL) {
            cv::convertScaleAbs(*slice, *slice,
                                255.0f / (maxValVolume - minValVolume),
                                255.0f * minValVolume / (minValVolume - maxValVolume));
            
            cv::threshold(*slice, helperMat, 77, 255, CV_THRESH_BINARY);
            
            cv::Canny(helperMat, helperMat, 20, 40, 3);
             
            std::vector<cv::vector<cv::Point> > contours;
             
            cv::findContours(helperMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1, cv::Point(0, 0));
             
            helperMat = cv::Scalar(0);
            for (size_t i = 0; i < contours.size(); ++ i) {
                if (contours.at(i).size() > 10) {
                    cv::drawContours(helperMat, contours, (int) i, cv::Scalar(255), 2, 8, cv::noArray(), 0, cv::Point());
                }
            }
            
            result = cv::Scalar(0);
            cv::bitwise_and(*slice, *slice, result, helperMat);
            result.copyTo(*slice);
        }
        
        for (int i = 0; i != ALL_EVENTS_COMPLETED; ++ i) {
            clReleaseEvent(eventList[i]);
        }
        
        clReleaseMemObject(fourier2dImageA);
        clReleaseMemObject(casBuf);
        clReleaseMemObject(tanBuf);
        clReleaseMemObject(radBuf);
        clEnqueueUnmapMemObject(_queue, sliceImage, sliceData, 0, nullptr, nullptr);
        
        clReleaseMemObject(sliceImage);
        
        visualize();
    }

    void Reconstructor::visualize() {
        cv::Mat slice(*_slicesOCL.at(0));

        size_t oneSliceSize = slice.elemSize() * slice.total();
        size_t sliceCount = _slicesOCL.size();

        uchar * mergedData = new uchar[oneSliceSize * sliceCount];
        uchar * posSlice = mergedData;
        
        for (const cv::Mat * slice : _slicesOCL) {
            memcpy(posSlice, slice->data, oneSliceSize);
            posSlice += oneSliceSize;
        }

        TextureInfo::TextureInfo texture;
        texture.mergedData = TextureInfo::MergedDataPointer(mergedData);

        texture.pixelTransferOptions.setAlignment((slice.step & 3) ? 1 : 4);
        texture.pixelTransferOptions.setRowLength((int) slice.step1());

        texture.size = TextureInfo::Size(slice.cols, slice.rows, sliceCount);

        texture.pixelType = QOpenGLTexture::UInt8;
        texture.textureFormat = QOpenGLTexture::R8_UNorm;
        texture.pixelFormat = QOpenGLTexture::Red;
        texture.target = QOpenGLTexture::Target3D;

        // how to calculate / get these ?
        QVector3D worldSpacings(0.3, 0.3, 1.0);

        VolumeInfo::Scaling scaling = scaleVector<float, QVector3D>(
                    texture.size.x() * worldSpacings.x(),
                    texture.size.y() * worldSpacings.y(),
                    texture.size.z() * worldSpacings.z()
                    ) * QVector3D(
                    1.0f / worldSpacings.x(),
                    1.0f / worldSpacings.y(),
                    1.0f / worldSpacings.z()
                    );

        QVariantMap blueprintOverallMap = _blueprint.toMap();
        QVariantList textureVolumeList = blueprintOverallMap["textures"].toList();
        QVariantMap textureVolume = textureVolumeList[0].toMap();

        textureVolume["desciptor"] = QVariant::fromValue(texture);

        textureVolumeList[0] = QVariant(textureVolume);

        blueprintOverallMap["textures"] = QVariant(textureVolumeList);

        QVariantList blueprintList = blueprintOverallMap["models"].toList();

        QVariantMap blueprintMap = blueprintList[0].toMap();
        QVariantMap blueprintParams = blueprintMap["params"].toMap();

        blueprintParams["size"] = QVariant(texture.size);
        blueprintParams["scaling"] = QVariant(scaling);

        blueprintMap["params"] = QVariant(blueprintParams);

        blueprintList[0] = QVariant(blueprintMap);
        blueprintOverallMap["models"] = QVariant(blueprintList);

        Message::SettingsMessage message("Reconstructor", "Scene");
        message.data["action"] = "add";
        message.data["blueprint"] = QVariant(blueprintOverallMap);

        send(message);
    }

    QVariant Reconstructor::files() const {
        return _imgFiles;
    }

    void Reconstructor::setFiles(const QVariant & files) {
        cv::Size imSize = cv::Size();
        cv::Mat readerMat;

        for (const QVariant & imgFile : files.value<QList<QUrl> >()) {
            readerMat = cv::imread(imgFile.toUrl().toLocalFile().toStdString(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_GRAYSCALE);
            readerMat.convertTo(readerMat, CV_32FC1, 1 / 256.0f);

            // all images must be the same size or resize
            if (imSize != cv::Size(0, 0)) {
                if (readerMat.size() != imSize) {
                    cv::resize(readerMat, readerMat, imSize);
                }
            }
            else {
                imSize = readerMat.size();
            }

            _src.push_back(readerMat);
        }

        reconstruct();
        //reconstructCPU();

        cv::namedWindow(SLICES_IMAGE_WINDOW);
        cv::namedWindow(SLICE_POSITION);

        showSliceWithNumber(0);

        _imgFiles = files;
        emit filesChanged();
    }

    void Reconstructor::reconstructCPU() {
        ReconstructionData reconstructionData;

        reconstructionData.src = &_src;
        reconstructionData.slices = &_slicesOCL;

        reconstructionData.sliceCount = _src.at(0).rows;

        cv::parallel_for_(cv::Range(0, reconstructionData.sliceCount), ReconstructorLoop(&reconstructionData));

        visualize();
    }

    void Reconstructor::nextSlice(const int & ds) {
        if (_slicesOCL.size()) {
            _sliceNumber += ds;
            _sliceNumber %= _slicesOCL.size();
            showSliceWithNumber(_sliceNumber);
        }
    }

    void Reconstructor::showSliceWithNumber(const int & sliceNumber) {
        cv::imshow(SLICES_IMAGE_WINDOW, *_slicesOCL.at(sliceNumber));

        cv::Mat slicePosition(_src.at(sliceNumber % _src.size()));
        cv::cvtColor(slicePosition, slicePosition, CV_GRAY2BGR);
        cv::rectangle(slicePosition, cv::Rect(0, sliceNumber, slicePosition.cols, 1), cv::Scalar(0, 0, 255));
        cv::imshow(SLICE_POSITION, slicePosition);
    }
}
