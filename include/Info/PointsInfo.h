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

        bool containts(const PointID & id);

        void togglePoint(const PointID & id);

        ModelPoint * operator [] (const PointID & id);

        QVector<ModelPoint *> points() const;

    private:
        QHash<PointID, ModelPoint *> _points;
    };
}

Q_DECLARE_METATYPE(PointsInfo::ModelPoints *)

#endif // POINTSINFO_H
