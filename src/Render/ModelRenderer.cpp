#include <QtQuick/QQuickWindow>

#include "Model/HeadModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size) {
        /* because renderer and its gl context live in the separate thread,
        * so we cannot simply add new model - gl funcs
        * (for buffers creation, shader compilation and so on)
        * would be called from wrong thread and app would simply crash.
        * we need to queue apropriate signals, but sending parameters don't
        * belong to qt family - need to use qRegisterMetaType
        */
        qRegisterMetaType<ModelInfo::BuffersVN>("ModelInfo::BuffersVN");
        qRegisterMetaType<ModelInfo::BuffersVT>("ModelInfo::BuffersVT");

        qRegisterMetaType<SliceInfo::Slices>("SliceInfo::Slices");
        qRegisterMetaType<PointsInfo::Point>("PointsInfo::Point");
    }

    ModelRenderer::~ModelRenderer() {
        cleanUp();
    }

    void ModelRenderer::setRotation(const QVector3D & rotation) {
        selectedScene()->setRotation(rotation);
        emit needToRedraw();
    }

    void ModelRenderer::setXRange(const ViewRangeInfo::ViewAxisRange & xRange) {
        selectedScene()->setXRange(xRange);
        emit needToRedraw();
    }

    void ModelRenderer::setYRange(const ViewRangeInfo::ViewAxisRange & yRange) {
        selectedScene()->setYRange(yRange);
        emit needToRedraw();
    }

    void ModelRenderer::setZRange(const ViewRangeInfo::ViewAxisRange & zRange) {
        selectedScene()->setZRange(zRange);
        emit needToRedraw();
    }

    void ModelRenderer::connectWithScene(Scene::AbstractScene * scene) {
        Scene::ModelScene * modelScene = qobject_cast<Scene::ModelScene *>(scene);

        QObject::connect(modelScene, &Scene::ModelScene::modelIDChanged, this, &Render::ModelRenderer::modelIDChanged, Qt::DirectConnection);
        QObject::connect(modelScene, &Scene::ModelScene::pointUpdated, this, &Render::ModelRenderer::pointUpdated, Qt::DirectConnection);

        AbstractRenderer::connectWithScene(scene);
    }

    void ModelRenderer::disconnectWithScene(Scene::AbstractScene * scene) {
        Scene::ModelScene * modelScene = qobject_cast<Scene::ModelScene *>(scene);

        QObject::disconnect(modelScene, &Scene::ModelScene::modelIDChanged, this, &Render::ModelRenderer::modelIDChanged);
        QObject::disconnect(modelScene, &Scene::ModelScene::pointUpdated, this, &Render::ModelRenderer::pointUpdated);

        AbstractRenderer::disconnectWithScene(scene);
    }

    void ModelRenderer::addStlModel(ModelInfo::BuffersVN buffers) {
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectedModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectedModelScene->addStlModel(buffers);

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
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectModelScene->addHeadModel(slices);

            locker.unlock();
            emit needToRedraw();
        }
        else {
            // sorry, different scene
            slices.texture.mergedData.clear();
        }
    }

    void ModelRenderer::addEvaluatorModel(const QSize & size,
                                          const qreal & stepX, const qreal & stepY,
                                          const QVector3D & color) {
        addEvaluatorModel(size.width(), size.height(), stepX, stepY, color);
    }

    void ModelRenderer::addEvaluatorModel(const int & width, const int & height,
                                          const qreal & stepX, const qreal & stepY,
                                          const QVector3D & color) {
        QMutexLocker locker(&renderMutex);

        if (Scene::ModelScene * selectedModelScene = qobject_cast<Scene::ModelScene *>(selectedScene())) {
            activateContext();

            selectedModelScene->addEvaluatorModel(width, height, stepX, stepY, color);

            locker.unlock();
            emit needToRedraw();
        }
    }

    void ModelRenderer::render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
}
