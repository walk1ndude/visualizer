#include <QtQuick/QQuickWindow>

#include "Model/HeadModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size),
        _zoomFactor(2.0) {

    }

    ModelRenderer::~ModelRenderer() {
        cleanUp();
    }

    void ModelRenderer::setTakeShot(const bool & takeShot) {
        _takeShot = takeShot;
    }

    void ModelRenderer::setRotation(const QVector3D & rotation) {
        _rotation = rotation;

        _viewPorts.rotate(_rotation.x(), _rotation.y(), _rotation.z());
        emit needToRedraw();
    }

    void ModelRenderer::setZoomFactor(const qreal & zoomFactor) {
        _viewPorts.zoom(zoomFactor);
        emit needToRedraw();
    }

    void ModelRenderer::setSRange(const ModelInfo::TexelAxisRange & sRange) {
        _selectedModel->setTexelAxisRange(sRange, ModelInfo::SAXIS);
        emit needToRedraw();
    }

    void ModelRenderer::setTRange(const ModelInfo::TexelAxisRange & tRange) {
        _selectedModel->setTexelAxisRange(tRange, ModelInfo::TAXIS);
        emit needToRedraw();
    }

    void ModelRenderer::setPRange(const ModelInfo::TexelAxisRange & pRange) {
        _selectedModel->setTexelAxisRange(pRange, ModelInfo::PAXIS);
        emit needToRedraw();
    }

    // need some good defaults here
    void ModelRenderer::initMaterials() {

    }

    void ModelRenderer::initLightSources() {

    }

    void ModelRenderer::addMaterial(const MaterialInfo::Emissive & emissive,
                                    const MaterialInfo::Diffuse & diffuse,
                                    const MaterialInfo::Specular & specular,
                                    const MaterialInfo::Shininess & shininess) {
        _materials.push_back(new MaterialInfo::Material(emissive, diffuse, specular, shininess));
    }

    void ModelRenderer::addLightSource(const LightInfo::Position & position,
                                       const LightInfo::Color & color,
                                       const LightInfo::AmbientIntensity & ambientIntensity) {
        _lightSources.push_back(new LightInfo::LightSource(position, color, ambientIntensity));
    }

    void ModelRenderer::initializeViewPorts() {
        ViewPorts viewPorts;

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ProjectionType>(
                        QRectF(0.5, 0.5, 0.5, 0.5), ViewPort::PERSPECTIVE)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ProjectionType>(
                        QRectF(0, 0.5, 0.5, 0.5), ViewPort::BOTTOM)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ProjectionType>(
                        QRectF(0, 0, 0.5, 0.5), ViewPort::FRONT)
                    );

        viewPorts.push_back(
                    QPair<QRectF, ViewPort::ProjectionType>(
                        QRectF(0.5, 0, 0.5, 0.5), ViewPort::LEFT)
                    );

        _viewPorts.setViewPorts(viewPorts, _surfaceSize);
    }

    void ModelRenderer::addTexture(TextureInfo::Texture & textureInfo) {
        QOpenGLTexture * texture = new QOpenGLTexture(textureInfo.target);

        _textures.insert(texture, textureInfo);
        _models.insert(_selectedModel, texture);
    }

    void ModelRenderer::bindTextures(Model::AbstractModel * model) {
        QMutexLocker locker(&_renderMutex);

        QMultiHash<Model::AbstractModel *, QOpenGLTexture *>::iterator it = _models.find(model);

        uint number = 0;

        // bind all textures, associatiated with given model
        while (it != _models.end() && it.key() == model) {
            it.value()->bind(number ++);
            _bindedTextures.push_back(it.value());
        }
    }

    void ModelRenderer::releaseTextures() {
        QMutexLocker locker(&_renderMutex);

        QListIterator<QOpenGLTexture *> it (_bindedTextures);

        uint number = 0;

        // release all textures, associatiated with given model
        while (it.hasNext()) {
            it.next()->release(number ++);
        }

        _bindedTextures.clear();
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

    void ModelRenderer::addStlModel(ModelInfo::BuffersVN & buffers) {
        Model::StlModel * model = new Model::StlModel;
        model->createModel(buffers);

        _models.insert(model, nullptr);
    }

    void ModelRenderer::render() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        _viewPorts.resize(_surfaceSize);

        ViewPort viewPort;
        ViewPortRect boundingRect;
        ViewPortsIterator itV (_viewPorts);

        Model::AbstractModel * model;
        QListIterator<Model::AbstractModel *> itM (_models.keys());

        // for each viewport
        while (itV.hasNext()) {
            viewPort = itV.next();

            boundingRect = viewPort.boundingRect();
            if (viewPort.projectionType() == ViewPort::LEFT) {
                _screenSaveRect = boundingRect;
            }

            glViewport(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());

            // draw each model
            while (itM.hasNext()) {
                model = itM.next();

                bindTextures(model);
                model->drawModel();
                releaseTextures();
            }

            itM.toFront();
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

        qDeleteAll(_textures.keys().begin(), _textures.keys().end());
        _textures.clear();

        QListIterator<Model::AbstractModel *> itM (_models.keys());

        while (itM.hasNext()) {
            delete itM.next();
        }
        _models.clear();

        qDeleteAll(_materials.begin(), _materials.end());
        _materials.clear();

        qDeleteAll(_lightSources.begin(), _lightSources.end());
        _lightSources.clear();
    }
}
