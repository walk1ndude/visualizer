#include "Info/TextureInfo.h"

namespace TextureInfo {
    TextureProgram::TextureProgram(QOpenGLShaderProgram * program,
                                   const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        _sampler(program->uniformLocation(shaderVariables.at(0))) {
    }

    void TextureProgram::setUniform(QOpenGLShaderProgram * program,
                                    const uint samplerNum) {
        program->setUniformValue(_sampler, samplerNum);
    }
}
