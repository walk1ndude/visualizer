#include <QtQuick/QQuickWindow>

#include "Model/HeadModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size) {
        initialize();
    }

    ModelRenderer::~ModelRenderer() {
        cleanUp();
    }

    void ModelRenderer::setTakeShot(const bool & takeShot) {
        _takeShot = takeShot;
    }

    void ModelRenderer::selectScene(Scene::AbstractScene * scene) {
        QMutexLocker locker(&_renderMutex);

        if (!scene->isInitialized()) {
            scene->initScene(_surfaceSize);
        }
        _selectedScene = scene;
        _sceneHistory.insert(scene);
    }

    void ModelRenderer::setRotation(const QVector3D & rotation) {
        _selectedScene->setRotation(rotation);
        emit needToRedraw();
    }

    void ModelRenderer::setZoomFactor(const qreal & zoomFactor) {
        _selectedScene->setZoomFactor(zoomFactor);
        emit needToRedraw();
    }

    void ModelRenderer::setXRange(const ModelInfo::ViewAxisRange & xRange) {
        _selectedScene->setXRange(xRange);
        emit needToRedraw();
    }

    void ModelRenderer::setYRange(const ModelInfo::ViewAxisRange & yRange) {
        _selectedScene->setYRange(yRange);
        emit needToRedraw();
    }

    void ModelRenderer::setZRange(const ModelInfo::ViewAxisRange & zRange) {
        _selectedScene->setZRange(zRange);
        emit needToRedraw();
    }

    void ModelRenderer::initialize() {

    }

    void ModelRenderer::addStlModel(ModelInfo::BuffersVN buffers) {
        QMutexLocker locker(&_renderMutex);

        if (Scene::ModelScene * selectedModelScene = dynamic_cast<Scene::ModelScene *>(_selectedScene)) {
            activateContext();

            selectedModelScene->addStlModel(buffers);

            // for now just dummy call to show hud
            emit appearedSmthToDraw();

            locker.unlock();
            emit needToRedraw();
        }
        else {
            // selected scene doesn't seem like modelScene -> don't risk to add to id, free buffers
            buffers.vertices.clear();
            buffers.indices.clear();
        }
    }

    void ModelRenderer::addHeadModel(SliceInfo::Slices slices) {
        QMutexLocker locker(&_renderMutex);

        if (Scene::ModelScene * selectModelScene = dynamic_cast<Scene::ModelScene *>(_selectedScene)) {
            activateContext();

            selectModelScene->addHeadModel(slices);

            emit appearedSmthToDraw();

            locker.unlock();
            emit needToRedraw();
        }
        else {
            // sorry, different scene
            slices.texture.mergedData.clear();
        }
    }

    void ModelRenderer::render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (_selectedScene) {
            _selectedScene->renderScene(_surfaceSize);
        }
    }

    void ModelRenderer::setPoint(const PointsInfo::Point & point) {
        if (Scene::ModelScene * selectModelScene = dynamic_cast<Scene::ModelScene *>(_selectedScene)) {
            selectModelScene->setPoint(point);
        }
    }

    void ModelRenderer::cleanUp() {
        QMutexLocker locker(&_renderMutex);

        activateContext();

        QSetIterator<Scene::AbstractScene *> it (_sceneHistory);

        while (it.hasNext()) {
            it.next()->cleanUp();
        }
    }
}
