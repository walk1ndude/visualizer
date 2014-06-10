#ifndef LIGHTINFO_H
#define LIGHTINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace LightInfo {
    using Position = QVector4D;
    using Color = QVector4D;

    using AmbientIntensity = GLfloat;

    class LightSource {
    public:
        Position position;
        Color color;
        AmbientIntensity ambientIntensity;

        LightSource(const Position & position,
                    const Color & color,
                    const AmbientIntensity & ambientIntensity) {
            this->position = position;
            this->color = color;
            this->ambientIntensity = ambientIntensity;
        }
    };

    class LightProgram {
    private:
        ShaderInfo::ShaderVariable shaderPosition;
        ShaderInfo::ShaderVariable shaderColor;
        ShaderInfo::ShaderVariable shaderAmbientIntensity;

    public:
        LightProgram(QOpenGLShaderProgram * program,
                     const QStringList & shaderVariables) {
            shaderPosition = program->uniformLocation(shaderVariables.at(0));
            shaderColor = program->uniformLocation(shaderVariables.at(1));
            shaderAmbientIntensity = program->uniformLocation(shaderVariables.at(2));
        }

        void setUniformValue(QOpenGLShaderProgram * program,
                             LightSource * lightSource) {
            program->setUniformValue(shaderPosition, lightSource->position);
            program->setUniformValue(shaderColor, lightSource->color);
            program->setUniformValue(shaderAmbientIntensity, lightSource->ambientIntensity);
        }
    };
}

#endif // LIGHTINFO_H
