#include "Info/TextureInfo.h"

static uint textureCount = 0;

namespace TextureInfo {
    QStringList Texture::initializationOrder = { "sampler" };

    Texture::Texture() :
        SceneInfo::SceneObject(SceneInfo::getNewID(textureCount)) {

    }

    Texture::Texture(QOpenGLTexture * texture, const SceneInfo::ObjectID & textureID) :
        SceneInfo::SceneObject(textureID) {
        _texture = texture;
    }

    Texture::Texture(const Params & params) :
        SceneInfo::SceneObject(params["id"].value<SceneInfo::ObjectID>()) {

        TextureInfo textureInfo = params["desciptor"].value<TextureInfo::TextureInfo>();

        _texture = new QOpenGLTexture(textureInfo.target);

        _texture->create();
        _texture->setFormat(textureInfo.textureFormat);
        _texture->setSize(textureInfo.size.x(), textureInfo.size.y(), textureInfo.size.z());

        _texture->allocateStorage();

        _texture->setMinMagFilters(QOpenGLTexture::LinearMipMapNearest, QOpenGLTexture::Linear);
        _texture->setWrapMode(QOpenGLTexture::ClampToBorder);

        _texture->setData(textureInfo.pixelFormat, textureInfo.pixelType,
                         (void *) textureInfo.mergedData.data(), &(textureInfo.pixelTransferOptions));
        _texture->generateMipMaps();
    }

    Texture::~Texture() {
        delete _texture;
    }

    QOpenGLTexture * Texture::texture() const {
        return _texture;
    }

    void Texture::setTexture(QOpenGLTexture * texture) {
        _texture = texture;

        emit textureChanged(texture);
    }

    TextureProgram::TextureProgram(QOpenGLShaderProgram * program,
                                   const ShaderInfo::ShaderVariablesNames & shaderVariables) :
        _sampler(program->uniformLocation(shaderVariables.at(0))) {
    }

    void TextureProgram::setUniform(QOpenGLShaderProgram * program,
                                    const uint samplerNum) {
        program->setUniformValue(_sampler, samplerNum);
    }
}
