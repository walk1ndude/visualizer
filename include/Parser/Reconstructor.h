#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include <QtCore/QStringList>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

#include "Parser/Helpers.hpp"
#include "Parser/AbstractParser.h"

#include "Info/VolumeInfo.h"

namespace Parser {
    class Reconstructor : public AbstractParser {
        Q_OBJECT
    public:
        explicit Reconstructor();
        ~Reconstructor();

        QVariant files() const;

    private:
        int _sliceNumber;

        QVariant _imgFiles;

        QVector<cv::Mat>_src;
        QVector<cv::Mat *>_slicesOCL;

        cl_context _context;
        cl_device_id _device_id;
        cl_program _programReconstruction;

        cl_kernel _gauss1dKernel;
        cl_kernel _calcTablesKernel;
        cl_kernel _dht1dTransposeKernel;
        cl_kernel _fourier2dKernel;
        cl_kernel _butterflyDht2dKernel;

        cl_command_queue _queue;
        
        bool _isOCLInitialized;

        void showSliceWithNumber(const int & sliceNumber);

        void initOCL();
        void releaseOCLResources();

        void reconstruct();
        void reconstructCPU();

        void visualize();

        void reset();

    public slots:
        virtual void setFiles(const QVariant & files) final;
        Q_INVOKABLE virtual void nextSlice(const int & ds);
    };
}

#endif // RECONSTRUCTOR_H
