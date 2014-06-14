#include <QtQuick/QQuickWindow>

#include "Model/HeadModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size),
        _selectedScene(nullptr) {
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
        _selectedScene->setXRange(yRange);
        emit needToRedraw();
    }

    void ModelRenderer::setZRange(const ModelInfo::ViewAxisRange & zRange) {
        _selectedScene->setXRange(zRange);
        emit needToRedraw();
    }

    void ModelRenderer::drawSlices(SliceInfo::SliceSettings sliceSettings) {/*
        _mergedData = sliceSettings.mergedData;
        _sliceDataType = sliceSettings.sliceDataType;

        if (updateContent()) {
            emit needToRedraw();
            return;
        }

        _scaling = sliceSettings.scaling;
        _size = sliceSettings.size;

        _step = QVector3D(1.0 / _size[0], 1.0 / _size[1], 1.0 / _size[2]);

        if (sliceSettings.rowLength) {
            _pixelOptionsHead.setAlignment(sliceSettings.alignment);
            _pixelOptionsHead.setRowLength(sliceSettings.rowLength);
        }

        _viewPorts.scale(QVector3D(_scaling[0], _scaling[1], _scaling[2]));
*/
        emit needToRedraw();
    }

    void ModelRenderer::initialize() {
        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void ModelRenderer::addStlModel(ModelInfo::BuffersVN buffers) {
        QMutexLocker locker(&_renderMutex);

        if (Scene::ModelScene * selectedModelScene = dynamic_cast<Scene::ModelScene *>(_selectedScene)) {
            activateContext();

            selectedModelScene->addStlModel(buffers);

            emit appearedSmthToDraw();

            locker.unlock();
            renderNext();
        }
        else {
            // selected scene doesn't seem like modelScene -> don't risk to add to id, free buffers
            buffers.vertices.clear();
            buffers.indices.clear();
        }
    }

    void ModelRenderer::render() {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (_selectedScene) {
            _selectedScene->renderScene(_surfaceSize);
        }
    }

    void ModelRenderer::updateTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                                        const QOpenGLTexture::TextureFormat & textureFormat,
                                        const QOpenGLTexture::PixelFormat & pixelFormat,
                                        const QOpenGLTexture::PixelType & pixelType,
                                        const QOpenGLPixelTransferOptions * pixelOptions) {


        QOpenGLTexture * tex = *texture;
        if (!tex) {
            tex = new QOpenGLTexture(QOpenGLTexture::Target3D);

            tex->create();

            //tex->setSize((int) _size[0],(int) _size[1],(int) _size[2]);
            tex->setFormat(textureFormat);

            tex->allocateStorage();
        }

        tex->setData(pixelFormat, pixelType, (void *) textureData.data(), pixelOptions);

        tex->setMinMagFilters(QOpenGLTexture::LinearMipMapNearest, QOpenGLTexture::Linear);
        tex->setWrapMode(QOpenGLTexture::ClampToBorder);

        tex->generateMipMaps();

        *texture = tex;

        textureData.clear();
    }

    void ModelRenderer::updateTextures() { /*
        switch (_sliceDataType) {
        case SliceInfo::Int8:
            updateTexture(&_textureHead, _mergedData, QOpenGLTexture::R8_UNorm,
                              QOpenGLTexture::Red, QOpenGLTexture::UInt8, &_pixelOptionsHead);
            break;
        default:
            updateTexture(&_textureHead, _mergedData, QOpenGLTexture::R16_UNorm,
                              QOpenGLTexture::Red, QOpenGLTexture::UInt16, &_pixelOptionsHead);
            break;
        } */
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
