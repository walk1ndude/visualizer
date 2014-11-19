#include <QtQuick/QQuickWindow>

#include "Model/VolumeModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size) {
        qRegisterMetaType<PointsInfo::UpdatedPoint>("PointsInfo::UpdatedPoint");
    }

    ModelRenderer::~ModelRenderer() {
        cleanUp();
    }

    void ModelRenderer::setRotation(const QVector3D & rotation) {
        selectedScene()->setRotation(rotation);
        emit redraw();
    }

    void ModelRenderer::setXRange(const ViewRangeInfo::ViewAxisRange & xRange) {
        selectedScene()->setXRange(xRange);
        emit redraw();
    }

    void ModelRenderer::setYRange(const ViewRangeInfo::ViewAxisRange & yRange) {
        selectedScene()->setYRange(yRange);
        emit redraw();
    }

    void ModelRenderer::setZRange(const ViewRangeInfo::ViewAxisRange & zRange) {
        selectedScene()->setZRange(zRange);
        emit redraw();
    }

    void ModelRenderer::setHuRange(const VolumeInfo::HuRange & huRange) {
        selectedScene()->setHuRange(huRange);
        emit redraw();
    }

    void ModelRenderer::connectWithScene(Scene::AbstractScene * scene) {
        Scene::ModelScene * modelScene = qobject_cast<Scene::ModelScene *>(scene);

        QObject::connect(modelScene, &Scene::ModelScene::modelIDChanged, this, &Render::ModelRenderer::modelIDChanged);
        QObject::connect(modelScene, &Scene::ModelScene::pointUpdated, this, &Render::ModelRenderer::pointUpdated);

        AbstractRenderer::connectWithScene(scene);
    }

    void ModelRenderer::disconnectWithScene(Scene::AbstractScene * scene) {
        Scene::ModelScene * modelScene = qobject_cast<Scene::ModelScene *>(scene);

        QObject::disconnect(modelScene, &Scene::ModelScene::modelIDChanged, this, &Render::ModelRenderer::modelIDChanged);
        QObject::disconnect(modelScene, &Scene::ModelScene::pointUpdated, this, &Render::ModelRenderer::pointUpdated);

        AbstractRenderer::disconnectWithScene(scene);
    }

    void ModelRenderer::addModel(ModelInfo::BuffersVN buffers) {
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectedModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectedModelScene->addModel(buffers);

            locker.unlock();
            emit redraw();
        }
        else {
            // selected scene doesn't seem like modelScene -> don't risk to add to id, free buffers
            buffers.vertices.clear();
            buffers.indices.clear();
        }
    }

    void ModelRenderer::addModel(VolumeInfo::Volume volume) {
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectModelScene->addModel(volume);

            locker.unlock();
            emit redraw();
        }
        else {
            // sorry, different scene
            volume.texture.mergedData.clear();
        }
    }

    void ModelRenderer::addModel(const QSize & size,
                                 const qreal & stepX, const qreal & stepY,
                                 const QVector3D & color) {
        addModel(size.width(), size.height(), stepX, stepY, color);
    }

    void ModelRenderer::addModel(const int & width, const int & height,
                                 const qreal & stepX, const qreal & stepY,
                                 const QVector3D & color) {
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectedModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectedModelScene->addModel(width, height, stepX, stepY, color);

            locker.unlock();
            emit redraw();
        }
    }

    void ModelRenderer::render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (selectedScene()) {
            if (!selectedScene()->isInitialized()) {
                selectedScene()->initializeScene();
            }

            selectedScene()->renderScene(surfaceSize());
        }
    }

    void ModelRenderer::addPoint(const PointsInfo::Point & point) {
        if (selectedScene()) {
            selectedScene()->addPoint(point);
        }
    }

    void ModelRenderer::hidePoint(const QString & point) {
        if (selectedScene()) {
            selectedScene()->togglePoint(point);
        }

        emit redraw();
    }
}
