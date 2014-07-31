#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

namespace PointsInfo {
    using PointName = QString;
    
    class Point {
    public:
        QPointF position;
        PointName name;
        QColor color;
        
        Point() { }
        
        Point(const QPointF & position,
              const PointName & name,
              const QColor & color) :
            position(position),
            name(name),
            color(color) {
            
        }
    };
    
    class CalcalutedPoint {
    public:
        QVector3D position;
        PointName name;

        CalcalutedPoint(const QVector3D & position,
                        const PointName & name) :
                    position(position),
                    name(name) {
            
                    }
    };

    class ModelPoint {
    public:
        QVector3D position;
        QColor color;
        uint polygonId;

        bool isPositionCalculated() {
            return _positionCalculated;
        }

        void positionCalculated() {
            _positionCalculated = true;
        }

        ModelPoint(const QVector3D & position,
                   const QColor & color,
                   const uint & polygonId = 0) :
            position(position),
            color(color),
            polygonId(polygonId),
            _positionCalculated(false) {

        }
    private:
        bool _positionCalculated;
    };


    class ModelPoints : public QHash<PointName, ModelPoint *> { };
/*
    class FacePointsProgram {
    private:
        QHash<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> _facePoints;
    public:
        FacePointsProgram() { }

        void addPoint(QOpenGLShaderProgram * program,
                      const ShaderInfo::ShaderVariableName & shaderVariableName) {
            _facePoints.insert(shaderVariableName, program->uniformLocation(shaderVariableName));
        }

        void setUniformValue(QOpenGLShaderProgram * program,
                             const FacePoints & facePoints,
                             const QMatrix4x4 & projViewMartix) {
            QHashIterator<ShaderInfo::ShaderVariableName , ShaderInfo::ShaderVariable> itFPShader(_facePoints);
            QHashIterator<ShaderInfo::ShaderVariableName , FacePoint *> itFP(facePoints);

            bool contains;

            while (itFP.hasNext()) {
                contains = false;
                itFP.next();

                while (itFPShader.hasNext() && !contains) {
                    itFPShader.next();
                    contains = itFPShader.key().contains(itFP.key());
                }

                if (contains && itFP.value()->isPositionCalculated()) {
                    program->setUniformValue(itFPShader.value(), projViewMartix * itFP.value()->position);
                }

                itFPShader.toFront();
            }
       }
    };*/
}


#endif // POINTSINFO_H
