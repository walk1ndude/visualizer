#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

#include "Viewport/Viewport.h"

namespace PointsInfo {
    using Position2D = QPointF;
    using Position3D = QVector3D;
    using Position4D = QVector4D;
    
    using PointID = QString;
    using Groups = QStringList;
    using Color = QColor;
    
    class Point {
    public:
        Position2D position;
        PointID name;
        Color color;
        
        Groups groups;

        Viewport::Viewport * viewport;
        
        Point();
        Point(const Position2D & position,
              const PointID & name,
              const Color & color,
              Viewport::Viewport * viewport);
    };
    
    class UpdatedPoint {
    public:
        Position3D position;
        PointID id;

        UpdatedPoint();

        UpdatedPoint(const Position3D & position,
                     const PointID & id,
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

        void queueToRecalculate();

        void positionCalculated(const Position3D & position);

        ModelPoint();

        ModelPoint(const Color & color,
                   const Groups & groups = Groups(),
                   const qreal & radius = 0.05f,
                   const bool & shown = true);
    private:
        bool _positionCalculated;
    };


    class ModelPoints {
    public:
        ModelPoints();
        ~ModelPoints();

        PointID key(ModelPoint * point) const;

        void insert(const PointID & id, ModelPoint * point);
        int size() const;

        void togglePoint(const PointID & point);

        ModelPoint * operator [] (const PointID & id);

        QVector<ModelPoint *> points() const;

    private:
        QHash<PointID, ModelPoint *> _points;
    };
}

Q_DECLARE_METATYPE(PointsInfo::Point)
Q_DECLARE_METATYPE(PointsInfo::UpdatedPoint)
Q_DECLARE_METATYPE(PointsInfo::ModelPoints)
Q_DECLARE_METATYPE(PointsInfo::ModelPoints *)

#endif // POINTSINFO_H
