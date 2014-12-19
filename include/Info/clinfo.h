#ifndef CLINFO_H
#define CLINFO_H

#include <QtCore/QString>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

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

namespace CLInfo {
    cl_context createContext(const cl_context_properties * props,
                             cl_device_type device_type,
                             cl_uint num_devices,
                             cl_device_id * device_id,
                             void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),
                             void * user_data,
                             cl_int * errcode_ret);

    cl_program createProgram(cl_context context, const QString & source);
}

#endif // CLINFO_H

