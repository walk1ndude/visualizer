#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(100.0f),
        _isInitialized(false),
        _mUnits(MM) {

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

    MaterialInfo::Material * AbstractScene::material(const MaterialInfo::MaterialID & id) const {
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

    LightInfo::LightSource * AbstractScene::lightSource(const LightInfo::LightID & id) const {
        LightInfo::LightSource * light = nullptr;

        if (id < 0 && !lightSources.isEmpty()) {
            light = lightSources.first();
        }

        for (LightInfo::LightSource * lightSource : lightSources) {
            if (lightSource->id() == id) {
                light = lightSource;
            }
        }

        return light;
    }
}
