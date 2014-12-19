#include "Plugin/InterconnectionFinderPlugin.h"

namespace Plugin {
    InterconnectionFinderPlugin::InterconnectionFinderPlugin() :
        AbstractOpenCLPlugin() {

    }

    void InterconnectionFinderPlugin::init() {
        context = CLInfo::createContext(nullptr, CL_DEVICE_TYPE_GPU, 1, const_cast<cl_device_id *>(&device_id), nullptr, nullptr, nullptr);

        queue = clCreateCommandQueue(context, device_id, 0, nullptr);

        program = CLInfo::createProgram(context, source());

        qDebug() << "Building OpenCL Program, error: " << clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);
/*
        _gauss1dKernel = clCreateKernel(_programReconstruction, "gauss1d", nullptr);
        _calcTablesKernel = clCreateKernel(_programReconstruction, "calcTables", nullptr);
        _fourier2dKernel = clCreateKernel(_programReconstruction, "fourier2d", nullptr);
        _dht1dTransposeKernel = clCreateKernel(_programReconstruction, "dht1dTranspose", nullptr);
        _butterflyDht2dKernel = clCreateKernel(_programReconstruction, "butterflyDht2d", nullptr);
        */
    }

    void InterconnectionFinderPlugin::recieve(const Message::SettingsMessage & message) {

    }

    void InterconnectionFinderPlugin::recieve(const QVariant & message) {

    }
}
