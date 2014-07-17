#ifndef POINTSINFO_H
#define POINTSINFO_H

#include "Info/ShaderInfo.h"

namespace PointsInfo {
    class Point {
    public:
        QVector2D position;
        QString name;

        Point(const QVector2D & position,
              const QString & name) {
            this->position = position;
            this->name = name;
        }
    };

    using Incisor = QVector2D;
    using LeftJoint = QVector2D;
    using RightJoint = QVector2D;
    using LeftProsthetic = QVector2D;
    using RightProsthetic = QVector2D;

    class FacePoints {
    public:
        Incisor incisor;
        LeftJoint leftJoint;
        RightJoint rightJoint;
        LeftProsthetic leftProsthetic;
        RightProsthetic rightProsthetic;

        // in the beginning there were no points
        FacePoints() {
            incisor = Incisor(-1.0f, -1.0f);
            leftJoint = LeftJoint(-1.0f, -1.0f);
            rightJoint = RightJoint(-1.0f, -1.0f);
            leftProsthetic = LeftProsthetic(-1.0f, -1.0f);
            rightProsthetic = RightProsthetic(-1.0f, -1.0f);
        }
    };

    class FacePointsProgram {
    private:
        ShaderInfo::ShaderVariable shaderIncisor;
        ShaderInfo::ShaderVariable shaderLeftJoint;
        ShaderInfo::ShaderVariable shaderRightJoint;
        ShaderInfo::ShaderVariable shaderLeftProsthetic;
        ShaderInfo::ShaderVariable shaderRightProsthetic;

    public:
        FacePointsProgram(QOpenGLShaderProgram * program,
                          const ShaderInfo::ShaderVariables & shaderVariables) {
            shaderIncisor = program->uniformLocation(shaderVariables.at(0));
            shaderLeftJoint = program->uniformLocation(shaderVariables.at(1));
            shaderRightJoint = program->uniformLocation(shaderVariables.at(2));
            shaderLeftProsthetic = program->uniformLocation(shaderVariables.at(3));
            shaderRightProsthetic = program->uniformLocation(shaderVariables.at(4));
        }

        void setUniformValue(QOpenGLShaderProgram * program,
                             FacePoints * facePoints) {
            program->setUniformValue(shaderIncisor, facePoints->incisor);
            program->setUniformValue(shaderLeftJoint, facePoints->leftJoint);
            program->setUniformValue(shaderRightJoint, facePoints->rightJoint);
            program->setUniformValue(shaderLeftProsthetic, facePoints->leftProsthetic);
            program->setUniformValue(shaderRightProsthetic, facePoints->rightProsthetic);
        }
    };
}


#endif // POINTSINFO_H
