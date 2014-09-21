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

    private:
        QList<AbstractModel *> _models;

        void reorder();
    };
}

#endif // MODELARRAY_H
