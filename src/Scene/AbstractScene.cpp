#include "Scene/AbstractScene.h"

namespace Scene {
    AbstractScene::AbstractScene(Viewport::ViewportArray ** viewportArray) :
        _viewportArray(viewportArray),
        _isInitialized(false) {
    }

    Viewport::ViewportArray * AbstractScene::viewportArray() {
        return *_viewportArray;
    }
}
