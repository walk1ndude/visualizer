#ifndef INTERCONNECTIONFINDERPLUGIN_H
#define INTERCONNECTIONFINDERPLUGIN_H

#include "Plugin/AbstractOpenCLPlugin.h"

namespace Plugin {
    class InterconnectionFinderPlugin : public AbstractOpenCLPlugin {
        Q_OBJECT
    public:
        explicit InterconnectionFinderPlugin();

    protected:
        void init(const Params & params);

    public slots:
        virtual void recieve(const Message::SettingsMessage & message);
        virtual void recieve(const QVariant & message);
    };
}

#endif // INTERCONNECTIONFINDERPLUGIN_H

