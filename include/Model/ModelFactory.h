#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include "Model/AbstractModel.h"

namespace Model {
    using ModelName = QString;
    using ModelParams = QVariantMap;

    using Count = int;

    using RequestedChildren = QHash<ModelName, Count>;

    class AbstractModel;

    class ModelFactory {
    public:
        virtual AbstractModel * createModel(const ModelParams & params = ModelParams()) = 0;
    };
}

#define REGISTER_TYPE(modelName) \
    namespace Model { \
        class modelName##Factory : public ModelFactory { \
        public: \
            modelName##Factory() { \
                AbstractModel::registerType(#modelName, this); \
            } \
            virtual AbstractModel * createModel(const ModelParams & params = ModelParams()) { \
                return new modelName(qvariant_cast<Scene::AbstractScene *>(params["scene"]), \
                                     qvariant_cast<AbstractModel *>(params["parent"]) \
                            ); \
            } \
        }; \
        static modelName##Factory global##modelName##Factory; \
    }

#endif // MODELFACTORY_H
