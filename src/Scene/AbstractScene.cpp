#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(100.0f),
        _isInitialized(false),
        _isLoaded(false),
        _mUnits(MM) {

    }

    bool AbstractScene::isInitialized() const {
        return _isInitialized;
    }

    void AbstractScene::loaded() {
        _isLoaded = true;
    }

    qreal AbstractScene::scalingFactor() const {
        return _scalingFactor;
    }

    void AbstractScene::setScalingFactor(const qreal & scalingFactor) {
        _scalingFactor = scalingFactor;

        emit scalingFactorChanged();
    }

    void AbstractScene::initializeScene() {
        if (!_isLoaded) {
            return;
        }

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
        for (MaterialInfo::Material * material : materials) {
            if (material->id() == id) {
                return material;
            }
        }

        return nullptr;
    }

    LightInfo::LightSource * AbstractScene::lightSource(const LightInfo::LightID & id) const {
        for (LightInfo::LightSource * lightSource : lightSources) {
            if (lightSource->id() == id) {
                return lightSource;
            }
        }

        return nullptr;
    }
}
