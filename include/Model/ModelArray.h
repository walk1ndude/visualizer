#ifndef MODELARRAY_H
#define MODELARRAY_H

#include "Model/AbstractModel.h"

namespace Model {
    class ModelArray {
    public:
        explicit ModelArray();
        ~ModelArray();

        virtual void append(AbstractModel * model);
        virtual void remove(AbstractModel * model);
        virtual void clear();

        virtual QList<AbstractModel *> array() final;

        AbstractModel * find(const int & id);

        virtual void invokeMethod(const int & id, const QString & name, const QVariantMap & params = QVariantMap());

    private:
        QList<AbstractModel *> _models;

        void reorder();
    };
}

#endif // MODELARRAY_H
