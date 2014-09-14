#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(1.0f),
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

    qreal AbstractScene::distance(const qreal & measure) const {
        return measure * _scalingFactor;
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
}
