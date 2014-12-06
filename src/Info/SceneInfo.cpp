#include "Info/SceneInfo.h"

namespace SceneInfo {
    ObjectID getNewID(uint & currentCounter) {
        return ObjectID::number(currentCounter ++);
    }

    SceneObject::SceneObject(const ObjectID & id, uint * currentCounter) {
        _id = !id.isEmpty() ? id : getNewID(*currentCounter);
    }

    ObjectID SceneObject::id() const {
        return _id;
    }

    void SceneObject::setID(const ObjectID & id) {
        _id = id;

        emit idChanged(id);
    }
}
