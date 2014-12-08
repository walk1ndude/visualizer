#include "Info/PointsInfo.h"

namespace PointsInfo {
    Point::Point() { }

    Point::Point(const Position2D & position,
      const PointID & name,
      const Color & color,
      Viewport::Viewport * viewport) :
    position(position),
    name(name),
    color(color),
    viewport(viewport) { }

    UpdatedPoint::UpdatedPoint() { }

    UpdatedPoint::UpdatedPoint(const Position3D & position,
                               const PointID & id,
                               const uint & modelId) :
        position(position),
        id(id),
        _modelId(modelId) {

    }

    uint UpdatedPoint::modelId() const {
        return _modelId;
    }

    bool ModelPoint::isPositionCalculated() const {
        return _positionCalculated;
    }

    void ModelPoint::positionCalculated(const Position3D & position) {
        this->position = position;
        _positionCalculated = true;
    }

    ModelPoint::ModelPoint() { }

    ModelPoint::ModelPoint(const Color & color,
                           const Groups & groups,
                           const qreal & radius,
                           const bool & shown) :
        radius(radius),
        color(color),
        shown(shown),
        groups(groups),
        _positionCalculated(false) {

    }

    ModelPoints::ModelPoints() { }

    ModelPoints::~ModelPoints() {
        qDeleteAll(_points);
    }

    PointID ModelPoints::key(ModelPoint * point) const {
        return _points.key(point);
    }

    void ModelPoints::insert(const PointID & id, ModelPoint * point) {
        _points.insert(id, point);
    }

    int ModelPoints::size() const {
        int count = 0;
        for (const ModelPoint * point : _points) {
            if (point->shown) {
                count ++;
            }
        }
        return count;
    }

    QVector<ModelPoint *> ModelPoints::points() const {
        QVector<ModelPoint *> points;
        for (ModelPoint * point : _points) {
            if (point->shown) {
                points.push_back(point);
            }
        }
        return points;
    }

    ModelPoint * ModelPoints::operator [](const PointID & id) {
        return _points[id];
    }

    void ModelPoints::togglePoint(const PointID & id) {
        if (_points.contains(id)) {
            _points[id]->shown = !_points[id]->shown;
        }
    }
}
