#ifndef MODELFACTORY_H
#define MODELFACTORY_H

#include "Model/AbstractModel.h"

namespace Model {
    using Type = QString;
    using Params = QVariantMap;

    using Count = int;

    using RequestedChildren = QHash<Type, Count>;

    class AbstractModel;

    class ModelFactory {
    public:
        virtual AbstractModel * createModel(const Params & params = Params()) = 0;
    };
}

#define REGISTER_TYPE(modelName) \
    namespace Model { \
        class modelName##Factory : public ModelFactory { \
        public: \
            modelName##Factory() { \
                AbstractModel::registerType(#modelName, this); \
            } \
            virtual AbstractModel * createModel(const Params & params = Params()) { \
                return new modelName(qvariant_cast<Scene::AbstractScene *>(params["scene"]), \
                                     qvariant_cast<AbstractModel *>(params["parent"]) \
                            ); \
            } \
        }; \
        static modelName##Factory global##modelName##Factory; \
    }

#endif // MODELFACTORY_H
