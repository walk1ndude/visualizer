#ifndef ABSTRACTOPENCLPLUGIN_H
#define ABSTRACTOPENCLPLUGIN_H

#include "Plugin/AbstractPlugin.h"

#include "Info/CLInfo.h"

namespace Plugin {
    class AbstractOpenCLPlugin : public AbstractPlugin {
        Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
        Q_OBJECT
    public:
        ~AbstractOpenCLPlugin();

        QString source() const;

    signals:
        void sourceChanged(const QString & source);

    protected:
        explicit AbstractOpenCLPlugin();

        virtual void init(const Params & params) = 0;

        cl_context context;

        cl_device_id device_id;

        cl_program program;

        cl_command_queue queue;

        virtual void releaseOCLResources();

    private:
        QString _source;

    public slots:
        virtual void setSource(const QString & source);

        virtual void recieve(const Message::SettingsMessage & message) = 0;
        virtual void recieve(const QVariant & message) = 0;
    };
}

#endif // ABSTRACTOPENCLPLUGIN_H

