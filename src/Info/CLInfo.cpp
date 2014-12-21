#include "Info/CLInfo.h"

#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>

#include <OpenGL.h>

namespace CLInfo {
    cl_context createContext(cl_device_type device_type,
                             cl_uint num_devices,
                             cl_device_id * device_id,
                             void (CL_CALLBACK * pfn_notify)(const char *, const void *, size_t, void *),
                             void * user_data,
                             cl_int * errcode_ret,
                             const bool & graphicsShared) {
        cl_platform_id * platforms;
        cl_uint platforms_n;

        clGetPlatformIDs(0, nullptr, &platforms_n);
        platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * (platforms_n + 1));
        clGetPlatformIDs(platforms_n, platforms, &platforms_n);

        clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, device_id, nullptr);

        cl_context_properties props[10];

        if (graphicsShared) {
#ifdef Q_OS_OSX
            CGLContextObj kCGLContext = CGLGetCurrentContext();
            CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);

            props[0] = CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE;
            props[1] = (cl_context_properties)kCGLShareGroup;
            props[2] = 0;
#endif

//TODO: shared context creation for other OS
        }

        cl_context context =

#ifdef AMD_BARTS
        clCreateContext(props, num_devices, device_id, pfn_notify, user_data, &errcode_ret);
#else
        clCreateContextFromType(graphicsShared ? props : nullptr, device_type, pfn_notify, user_data, errcode_ret);
#endif
        free(platforms);

        return context;
    }

    cl_program createProgram(cl_context context, const QString & source) {
        QFile programFile(source);

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

        cl_program program = clCreateProgramWithSource(context, 1, (const char **) &programText, (const size_t *) &programLength, nullptr);

        free(programText);

        return program;
    }
}
