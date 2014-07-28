#ifndef ABSTRACTMODELWITHPOINTS_H
#define ABSTRACTMODELWITHPOINTS_H

#include "Model/AbstractModel.h"
#include "Model/PointsModel.h"

#include "Info/PointsInfo.h"

namespace Model {
    class AbstractModelWithPoints : public AbstractModel {
        Q_OBJECT
    public:
        virtual void addPoint(const QString & name, const PointsInfo::FacePoint & point);

    protected:
        AbstractModelWithPoints(PointsModel * points = nullptr,
                                AbstractModel * parent = nullptr,
                                const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles());

        virtual PointsModel * pointsModel() final;

    private:
        PointsModel * _points;

        PointsInfo::FacePoints _pointsInfo;
    };
}

#endif // ABSTRACTMODELWITHPOINTS_H
