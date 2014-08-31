#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

#include "Viewport/Viewport.h"

namespace PointsInfo {
    using PointName = QString;
    
    class Point {
    public:
        QPointF position;
        PointName name;
        QColor color;

        Viewport::Viewport * viewport;
        
        Point() { }
        
        Point(const QPointF & position,
              const PointName & name,
              const QColor & color,
              Viewport::Viewport * viewport) :
            position(position),
            name(name),
            color(color),
            viewport(viewport) {
            
        }
    };
    
    class UpdatedPoint {
    public:
        QVector3D position;
        PointName name;

        UpdatedPoint() { }

        UpdatedPoint(const QVector3D & position,
                     const PointName & name,
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
        QVector3D position;
        qreal radius;
        QColor color;
        uint polygonId;

        Viewport::Viewport * viewport;

        bool isPositionCalculated() const {
            return _positionCalculated;
        }

        void positionCalculated(const QVector4D & position) {
            this->position = QVector3D(position);
            _positionCalculated = true;
        }

        ModelPoint(const QVector3D & position,
                   const QColor & color,
                   Viewport::Viewport * viewport,
                   const uint & polygonId = 0,
                   const qreal & radius = 0.2f) :
            position(position),
            radius(radius),
            color(color),
            polygonId(polygonId),
            viewport(viewport),
            _positionCalculated(false) {

        }
    private:
        bool _positionCalculated;
    };


    class ModelPoints : public QHash<PointName, ModelPoint *> { };
}


#endif // POINTSINFO_H
