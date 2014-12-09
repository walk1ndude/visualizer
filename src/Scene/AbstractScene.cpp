#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(100.0f),
        _isInitialized(false),
        _mUnits(MM) {

    }

    AbstractScene::~AbstractScene() {

    }

    void AbstractScene::cleanUp() {
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

    Material * AbstractScene::material(const ObjectID & id) const {
        return materials.find(id);
    }

    LightSource * AbstractScene::lightSource(const ObjectID & id) const {
        return lightSources.find(id);
    }

    Texture * AbstractScene::texture(const ObjectID & id) const {
        return textures.find(id);
    }
}
