#include "Model/AbstractModelWithPoints.h"

namespace Model {
    AbstractModelWithPoints::AbstractModelWithPoints(PointsModel * points, AbstractModel * parent,
                                                     const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {
        _points = points;
    }

    PointsModel * AbstractModelWithPoints::pointsModel() {
        return _points;
    }

    void AbstractModelWithPoints::addPoint(const QString & name, const PointsInfo::FacePoint & point) {
            //Poin * pointsInThisModel = nullptr;


            //        while (it.hasNext() && )

    }

}
