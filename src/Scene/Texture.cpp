#include "Scene/Texture.h"

static uint textureCount = 0;

namespace Scene {
    QStringList Texture::initializationOrder = { "sampler" };

    Texture::Texture() :
        SceneObject(getNewID(textureCount)) {

    }

    Texture::Texture(QOpenGLTexture * texture, const ObjectID & textureID) :
        SceneObject(textureID) {
        _texture = texture;
    }

    Texture::Texture(const TextureInfo::Params & params) :
        SceneObject(params["id"].value<ObjectID>()) {

        TextureInfo::TextureInfo textureInfo = params["desciptor"].value<TextureInfo::TextureInfo>();

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
        _texture->destroy();
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

