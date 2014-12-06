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

    AbstractModel * ModelArray::find(const uint & id) {
        AbstractModel * found = nullptr;
        AbstractModel * current = nullptr;

        QListIterator<AbstractModel *> it(_models);
/*
        while (!found && it.hasNext()) {
            current = it.next();
            found = (id == current->id()) ? current : found;
        }
*/
        return found;
    }

    AbstractModel * ModelArray::operator [] (const uint & id) {
        return find(id);
    }

    void ModelArray::invokeMethod(const int & id, const QString & name,
                                  const QVariantMap & params) {
        if (AbstractModel * model = find(id)) {
            model->invoke(name, params);
        }
    }
}
