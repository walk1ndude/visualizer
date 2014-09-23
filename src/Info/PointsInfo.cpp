#include "Info/PointsInfo.h"

namespace PointsInfo {
    Point::Point() { }

    Point::Point(const Position2D & position,
      const Name & name,
      const Color & color,
      Viewport::Viewport * viewport) :
    position(position),
    name(name),
    color(color),
    viewport(viewport) { }

    UpdatedPoint::UpdatedPoint() { }

    UpdatedPoint::UpdatedPoint(const Position3D & position,
                               const Name & name,
                               const uint & modelId) :
        position(position),
        name(name),
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

    ModelPoint::ModelPoint(const Position3D & position,
                           const Color & color,
                           Viewport::Viewport * viewport,
                           const Groups & groups,
                           const qreal & radius,
                           const bool & shown) :
        position(position),
        radius(radius),
        color(color),
        shown(shown),
        groups(groups),
        viewport(viewport),
        _positionCalculated(false) {

    }

    ModelPoints::~ModelPoints() {
        qDeleteAll(_points.begin(), _points.end());
    }

    Name ModelPoints::key(ModelPoint * point) const {
        return _points.key(point);
    }

    void ModelPoints::insert(const Name & name, ModelPoint * point) {
        _points.insert(name, point);
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

    void ModelPoints::togglePoint(const Name & point) {
        if (_points.find(point) != _points.end()) {
            _points[point]->shown = !_points[point]->shown;
        }
    }
}
