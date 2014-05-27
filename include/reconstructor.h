#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

#include "sliceinfo.h"

class Reconstructor : public QObject {
    Q_OBJECT
public:
    explicit Reconstructor(QObject * parent = 0);
    ~Reconstructor();

private:
    int _sliceNumber;

    std::vector<cv::Mat>_src;
    std::vector<cv::Mat>_slicesOCL;
    std::vector<cv::Mat *>_slices;

    cl_context _context;
    cl_device_id _device_id;
    cl_program _programSlice;

    cl_kernel _calcCasKernel;
    cl_kernel _calcTrigKernel;
    cl_kernel _dht1dTransposeKernel;
    cl_kernel _fourier2dKernel;
    cl_kernel _butterflyDht2dKernel;

    cl_command_queue _queue;

    void reset(const int & newSize = 0);
    void resetV(std::vector<cv::Mat *> & vec, const int & newSize = 0);

    void showSliceWithNumber(const int & sliceNumber);

    void initOpenCL();
    void reconstruct();

signals:
    void slicesProcessed(SliceInfo::SliceSettings sliceSettings);

public slots:
    void readFiles(QStringList fileNames);
    void changeSliceNumber(const int & ds);
};

#endif // RECONSTRUCTOR_H
