#include "Model/ModelArray.h"

namespace Model {
    ModelArray::ModelArray() {

    }

    ModelArray::~ModelArray() {
        clear();
    }

    void ModelArray::append(AbstractModel * model) {
        _models.append(model);

        reorder();
    }

    void ModelArray::remove(AbstractModel * model) {
        _models.removeAll(model);
    }

    void ModelArray::clear() {
        qDeleteAll(_models.begin(), _models.end());
        _models.clear();
    }

    QList<AbstractModel *> ModelArray::array() {
        return _models;
    }

    void ModelArray::reorder() {
        QList<AbstractModel *> withoutDepth;
        QList<AbstractModel *> withDepth;

        for (AbstractModel * model : _models) {
            if (!model->hasDepth()) {
                withoutDepth.append(model);
            }
            else {
                withDepth.append(model);
            }
        }

        _models = QList<AbstractModel *>(withoutDepth + withDepth);
    }
}
