#include "Model/PointsModel.h"

namespace Model {
    PointsModel::PointsModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {

    }

    void PointsModel::init(const PointsInfo::FacePoints & facePoints) {
        update(facePoints);
    }

    void PointsModel::update(const PointsInfo::FacePoints & facePoints) {
        _vertexCount = facePoints.size();

        ModelInfo::VerticesVCPPtr vertices = new ModelInfo::VerticesVCP;

        foreach (const PointsInfo::FacePoint * facePoint, facePoints) {
            vertices->push_back(ModelInfo::VertexVCP(
                                    facePoint->position.x(),
                                    facePoint->position.y(),
                                    facePoint->position.z(),
                                    facePoint->color.redF(),
                                    facePoint->color.greenF(),
                                    facePoint->color.blueF(),
                                    facePoint->polygonId
                                    ));
        }
    }
}
