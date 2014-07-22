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

        bool depthTested() {
            return _depthTested;
        }

        void depthTestCompleted() {
            _depthTested = true;
        }

        bool isPositionCalculated() {
            return _positionCalculated;
        }

        void positionCalculated() {
            _positionCalculated = true;
        }

        FacePoint(const QVector4D & position,
                  const QColor & color) {
            this->position = position;
            this->color = color;

            _depthTested = false;
            _positionCalculated = false;
        }
    private:
        bool _depthTested;
        bool _positionCalculated;
    };


    class FacePoints : public QHash<QString, FacePoint *> { };

    class FacePointsProgram {
    private:
        QHash<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> _facePoints;
    public:
        FacePointsProgram() { }

        void addPoint(QOpenGLShaderProgram * program,
                      const ShaderInfo::ShaderVariableName & shaderVariable) {
            _facePoints.insert(shaderVariable, program->uniformLocation(shaderVariable));
        }

        void setUniformValue(QOpenGLShaderProgram * program,
                             const FacePoints & facePoints) {
            QHashIterator<QString, ShaderInfo::ShaderVariable> itFPShader(_facePoints);
            QHashIterator<QString, FacePoint *> itFP(facePoints);

            bool contains;

            while (itFP.hasNext()) {
                contains = false;
                itFP.next();

                while (itFPShader.hasNext() && !contains) {
                    itFPShader.next();
                    contains = itFPShader.key().contains(itFP.key());
                }

                if (contains && itFP.value()->isPositionCalculated()) {
                    program->setUniformValue(itFPShader.value(), itFP.value()->position);
                }

                itFPShader.toFront();
            }
       }
    };
}


#endif // POINTSINFO_H
