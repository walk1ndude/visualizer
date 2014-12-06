#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(100.0f),
        _isInitialized(false),
        _mUnits(MM) {

    }

    AbstractScene::~AbstractScene() {
        cleanUp();
    }

    void AbstractScene::cleanUp() {
        qDeleteAll(lightSources);
        qDeleteAll(materials);
        qDeleteAll(textures);

        lightSources.clear();
        materials.clear();
        textures.clear();
    }

    bool AbstractScene::isInitialized() const {
        return _isInitialized;
    }

    qreal AbstractScene::scalingFactor() const {
        return _scalingFactor;
    }

    void AbstractScene::setScalingFactor(const qreal & scalingFactor) {
        _scalingFactor = scalingFactor;

        emit scalingFactorChanged();
    }

    void AbstractScene::initializeScene() {
        initScene();

        _isInitialized = true;
    }

    AbstractScene::MeasureUnits AbstractScene::measureUnits() const {
        return _mUnits;
    }

    void AbstractScene::setMeasureUnits(const AbstractScene::MeasureUnits & mUnits) {
        _mUnits = mUnits;

        emit measureUnitsChanged();
    }

    MaterialInfo::Material * AbstractScene::material(const SceneInfo::ObjectID & id) const {
        MaterialInfo::Material * mat = nullptr;

        if (id.isEmpty() && !materials.isEmpty()) {
            mat = materials.first();
        }

        for (MaterialInfo::Material * material : materials) {
            if (material->id() == id) {
                mat = material;
                break;
            }
        }

        return mat;
    }

    LightInfo::LightSource * AbstractScene::lightSource(const SceneInfo::ObjectID & id) const {
        LightInfo::LightSource * light = nullptr;

        if (id.isEmpty() && !lightSources.isEmpty()) {
            light = lightSources.first();
        }

        for (LightInfo::LightSource * lightSource : lightSources) {
            if (lightSource->id() == id) {
                light = lightSource;
            }
        }

        return light;
    }

    TextureInfo::Texture * AbstractScene::texture(const SceneInfo::ObjectID & id) const {
        TextureInfo::Texture * tex = nullptr;

        if (id.isEmpty() && !textures.isEmpty()) {
            tex = textures.first();
        }

        for (TextureInfo::Texture * _texture : textures) {
            if (_texture->id() == id) {
                tex = _texture;
            }
        }

        return tex;
    }
}
