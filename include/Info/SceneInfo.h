#ifndef SCENEINFO_H
#define SCENEINFO_H

#include "Info/Info.h"

namespace SceneInfo {
    using Blueprint = QVariantMap;

    using ObjectID = QString;

    ObjectID getNewID(uint & currentCounter);

    class SceneObject : public QObject {
        Q_PROPERTY(QString name READ id WRITE setID NOTIFY idChanged)
        Q_OBJECT

    public:
        explicit SceneObject(const ObjectID & id, uint * currentCounter = nullptr);

        ObjectID id() const;

    private:
        ObjectID _id;

    signals:
        void idChanged(const ObjectID & id);

    public slots:
        virtual void setID(const ObjectID & id);
    };
}

#endif // SCENEINFO_H
