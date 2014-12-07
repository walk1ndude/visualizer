#ifndef MODELARRAY_H
#define MODELARRAY_H

#include "Model/AbstractModel.h"

#include "Scene/SceneObject.h"

namespace Model {
    class ModelArray {
    public:
        explicit ModelArray();
        ~ModelArray();

        virtual void append(AbstractModel * model);
        virtual void remove(AbstractModel * model);
        virtual void clear();

        virtual QList<AbstractModel *> array() final;

        AbstractModel * find(const Scene::ObjectID & id);
        AbstractModel * operator[] (const Scene::ObjectID & id);

        virtual void invokeMethod(const int & id, const QString & name, const QVariantMap & params = QVariantMap());

    private:
        QList<AbstractModel *> _models;

        AbstractModel * _selectedModel;

        void reorder();
    };
}

#endif // MODELARRAY_H
