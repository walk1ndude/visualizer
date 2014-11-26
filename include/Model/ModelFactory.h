#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include "Model/AbstractModel.h"

namespace Model {
    class AbstractModel;

    class ModelFactory {
    public:
        virtual AbstractModel * createModel(Scene::AbstractScene * scene, AbstractModel * parent = nullptr) = 0;
    };
}

#define REGISTER_TYPE(modelName) \
    namespace Model { \
        class modelName##Factory : public ModelFactory { \
        public: \
            modelName##Factory() { \
                AbstractModel::registerType(#modelName, this); \
            } \
            virtual AbstractModel * createModel(Scene::AbstractScene * scene, AbstractModel * parent = nullptr) { \
                return new modelName(scene, parent); \
            } \
        }; \
        static modelName##Factory global##modelName##Factory; \
    }

#endif // MODELFACTORY_H
