#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include <QtCore/QStringList>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

#include "Info/SliceInfo.h"

namespace Parser {
    class Reconstructor : public QObject {
        Q_OBJECT
    public:
        explicit Reconstructor(QObject * parent = 0);
        ~Reconstructor();

        void reset();

    private:
        int _sliceNumber;

        QVector<cv::Mat>_src;
        QVector<cv::Mat *>_slicesOCL;

        cl_context _context;
        cl_device_id _device_id;
        cl_program _programSlice;

        cl_kernel _gauss1dKernel;
        cl_kernel _calcTablesKernel;
        cl_kernel _dht1dTransposeKernel;
        cl_kernel _fourier2dKernel;
        cl_kernel _butterflyDht2dKernel;

        cl_command_queue _queue;

        void showSliceWithNumber(const int & sliceNumber);

        void initOpenCL();

        void reconstruct();
        void reconstructCPU();

        void visualize();

    signals:
        void slicesProcessed(SliceInfo::Slices slices);

    public slots:
        void readFiles(const QStringList & fileNames);
        void changeSliceNumber(const int & ds);
    };
}

#endif // RECONSTRUCTOR_H
