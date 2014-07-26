#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

namespace PointsInfo {
    class Point {
    public:
        QPointF position;
        QString name;
        QColor color;

        Point() { }

        Point(const QPointF & position,
              const QString & name,
              const QColor & color) {
            this->position = position;
            this->name = name;
            this->color = color;
        }
    };

    class FacePoint {
    public:
        QVector4D position;
        QColor color;
        uint polygonId;

        bool isPositionCalculated() {
            return _positionCalculated;
        }

        void positionCalculated() {
            _positionCalculated = true;
        }

        FacePoint(const QVector4D & position,
                  const QColor & color,
                  const uint & polygonId = 0) {
            this->position = position;
            this->color = color;
            this->polygonId = polygonId;

            _positionCalculated = false;
        }
    private:
        bool _positionCalculated;
    };


    class FacePoints : public QHash<QString, FacePoint *> { };

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
    };
}


#endif // POINTSINFO_H
