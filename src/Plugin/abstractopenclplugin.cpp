#include "Plugin/AbstractOpenCLPlugin.h"

namespace Plugin {
    AbstractOpenCLPlugin::AbstractOpenCLPlugin() :
        AbstractPlugin() {
    }

    AbstractOpenCLPlugin::~AbstractOpenCLPlugin() {
        releaseOCLResources();
    }

    QString AbstractOpenCLPlugin::source() const {
        return _source;
    }

    void AbstractOpenCLPlugin::setSource(const QString & source) {
        _source = source;

        emit sourceChanged(source);
    }

    void AbstractOpenCLPlugin::releaseOCLResources() {
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
#ifdef CL_VERSION_1_2
        clReleaseDevice(device_id);
#endif
        clReleaseContext(context);
    }
}
