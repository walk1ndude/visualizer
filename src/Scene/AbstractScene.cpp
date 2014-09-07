#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene() :
        _isInitialized(false) {

    }

    bool AbstractScene::isInitialized() {
        return _isInitialized;
    }

    Viewport::ViewportArray * AbstractScene::viewportArray() const {
        return _viewportArray;
    }

    void AbstractScene::setViewportArray(Viewport::ViewportArray * viewportArray) {
        _viewportArray = viewportArray;

        emit viewportArrayChanged();
    }

    void AbstractScene::initializeScene() {
        initScene();

        _isInitialized = true;
    }
}
