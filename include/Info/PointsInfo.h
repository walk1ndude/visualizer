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
        
        Point();
        Point(const Position2D & position,
              const Name & name,
              const Color & color,
              Viewport::Viewport * viewport);
    };
    
    class UpdatedPoint {
    public:
        Position3D position;
        Name name;

        UpdatedPoint();

        UpdatedPoint(const Position3D & position,
                     const Name & name,
                     const uint & modelId);

        uint modelId() const;

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

        bool isPositionCalculated() const;

        void positionCalculated(const QVector4D & position);

        ModelPoint(const Position3D & position,
                   const Color & color,
                   Viewport::Viewport * viewport,
                   const Groups & groups = Groups(),
                   const qreal & radius = 0.05f,
                   const bool & shown = true);
    private:
        bool _positionCalculated;
    };


    class ModelPoints {
    public:
        ~ModelPoints();

        Name key(ModelPoint * point) const;

        void insert(const Name & name, ModelPoint * point);
        int size() const;

        void hidePoint(const Name & point);

        QVector<ModelPoint *> points() const;

    private:
        QHash<Name, ModelPoint *> _points;
    };

}


#endif // POINTSINFO_H
