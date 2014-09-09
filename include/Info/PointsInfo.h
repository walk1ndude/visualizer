#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

#include "Viewport/Viewport.h"

namespace PointsInfo {
    using Position2D = QPointF;
    using Position3D = QVector3D;
    
    using Name = QString;
    using Groups = QStringList;
    using Color = QColor;
    
    class Point {
    public:
        Position2D position;
        Name name;
        Color color;
        
        Groups groups;

        Viewport::Viewport * viewport;
        
        Point() { }
        
        Point(const Position2D & position,
              const Name & name,
              const Color & color,
              Viewport::Viewport * viewport) :
            position(position),
            name(name),
            color(color),
            viewport(viewport) {
            
        }
    };
    
    class UpdatedPoint {
    public:
        Position3D position;
        Name name;

        UpdatedPoint() { }

        UpdatedPoint(const Position3D & position,
                     const Name & name,
                     const uint & modelId) :
                    position(position),
                    name(name),
                    _modelId(modelId) {
            
                    }

        uint modelId() const {
            return _modelId;
        }

    private:
        uint _modelId;
    };

    class ModelPoint {
    public:
        Position3D position;
        qreal radius;
        Color color;

        bool shown;
        
        Groups groups;

        Viewport::Viewport * viewport;

        bool isPositionCalculated() const {
            return _positionCalculated;
        }

        void positionCalculated(const QVector4D & position) {
            this->position = Position3D(position);
            _positionCalculated = true;
        }

        ModelPoint(const Position3D & position,
                   const Color & color,
                   Viewport::Viewport * viewport,
                   const Groups & groups = Groups(),
                   const qreal & radius = 0.05f,
                   const bool & shown = true) :
            position(position),
            radius(radius),
            color(color),
            shown(shown),
            groups(groups),
            viewport(viewport),
            _positionCalculated(false) {

        }
    private:
        bool _positionCalculated;
    };


    class ModelPoints : public QHash<Name, ModelPoint *> {
    public:
        int sizeShown() {
            int count = 0;
            for (ModelPoint * point : *this) {
                if (point->shown) {
                    count ++;
                }
            }
            return count;
        }
    };
}


#endif // POINTSINFO_H
