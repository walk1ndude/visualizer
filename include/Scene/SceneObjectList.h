#ifndef SCENEOBJECTLIST_H
#define SCENEOBJECTLIST_H

#include "Scene/SceneObject.h"

namespace Scene {
    template <class SObj>
    class SceneObjectList {
    public:
        SceneObjectList() :
            _selectedObject(nullptr) {

        }

        ~SceneObjectList() {
            clear();
        }

        SObj selectedObject() const {
            return _selectedObject;
        }

        virtual void selectObject(const ObjectID & id) {
            find(id);
        }

        virtual void selectObject(SObj obj) {
            _selectedObject = _objects.contains(obj) ? obj : nullptr;
        }

        SObj find(const ObjectID & id) const {
            SObj obj = nullptr;

            if ((id.isEmpty() || id.startsWith("currentModel")) && !_objects.isEmpty()) {
                obj = _selectedObject;
            }

            for (SObj object : _objects) {
                if (object->id() == id) {
                   obj = object;
                   break;
                }
            }

            return obj;
        }

        SObj operator [] (const ObjectID & id) const {
            return find(id);
        }

        void append(SObj object) {
            _selectedObject = object;

            _objects.append(object);
        }

        virtual void clear() {
            qDeleteAll(_objects);

            _selectedObject = nullptr;
        }

        QList<SObj> list() const {
            return _objects;
        }

    private:
        SObj _selectedObject;

        QList<SObj> _objects;
    };
}

#endif // SCENEOBJECTLIST_H
