#include "Scene/AbstractScene.h"

namespace Scene {
    Viewport::ViewportArray * AbstractScene::viewportArray() const {
        return _viewportArray;
    }

    void AbstractScene::setViewportArray(Viewport::ViewportArray * viewportArray) {
        _viewportArray = viewportArray;

        emit viewportArrayChanged();
    }
}
