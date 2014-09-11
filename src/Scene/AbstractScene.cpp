#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _scalingFactor(1.0f),
        _isInitialized(false) {

    }

    bool AbstractScene::isInitialized() const {
        return _isInitialized;
    }

    Viewport::ViewportArray * AbstractScene::viewportArray() const {
        return _viewportArray;
    }

    void AbstractScene::setViewportArray(Viewport::ViewportArray * viewportArray) {
        _viewportArray = viewportArray;

        emit viewportArrayChanged();
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
}
