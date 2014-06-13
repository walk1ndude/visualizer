#include <QtQuick/QQuickWindow>

#include "Model/HeadModel.h"
#include "Model/StlModel.h"

#include "Render/ModelRenderer.h"

namespace Render {
    ModelRenderer::ModelRenderer(QOpenGLContext * context, const QSize & size) :
        AbstractRenderer(context, size),
        _zoomFactor(2.0) {
        initialize();
        initializeViewPorts();
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

    void ModelRenderer::setSRange(const ModelInfo::ViewAxisRange & xRange) {
        _selectedModel->setViewAxisRange(xRange, ModelInfo::XAXIS);
        emit needToRedraw();
    }

    void ModelRenderer::setTRange(const ModelInfo::ViewAxisRange & yRange) {
        _selectedModel->setViewAxisRange(yRange, ModelInfo::YAXIS);
        emit needToRedraw();
    }

    void ModelRenderer::setPRange(const ModelInfo::ViewAxisRange & zRange) {
        _selectedModel->setViewAxisRange(zRange, ModelInfo::ZAXIS);
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
        _texturesInModel.insert(_selectedModel, texture);
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
        addLightSource(LightInfo::Position(10.0, 10.0, -10.0, 1.0),
                       LightInfo::Position(1.0, 1.0, 1.0, 1.0),
                       LightInfo::AmbientIntensity((GLfloat) 4.3));

        addMaterial(MaterialInfo::Emissive(0.8, 0.8, 0.8, 0.8),
                    MaterialInfo::Diffuse(1.0, 1.0, 1.0, 0.8),
                    MaterialInfo::Specular(0.7, 0.7, 0.7, 0.7),
                    MaterialInfo::Shininess(50.0));

        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void ModelRenderer::addStlModel(ModelInfo::BuffersVN buffers) {
        activateContext();

        Model::StlModel * model = new Model::StlModel;

        model->createModel<ModelInfo::BuffersVN>(buffers);

        model->addLightSource(_lightSources.at(0),
                              ShaderInfo::ShaderVariables() << "lightSource.location" << "lightSource.color" <<
                              "lightSource.ambientIntensity");

        model->addMaterial(_materials.at(0),
                           ShaderInfo::ShaderVariables() << "stlMaterial.emissive" << "stlMaterial.diffuse" <<
                           "stlMaterial.specular" << "stlMaterial.shininess");

        model->setViewRange(ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ModelInfo::ViewAxisRange(-1.0, 1.0),
                            ShaderInfo::ShaderVariables() << "xRange" << "yRange" << "zRange");

        _renderMutex.lock();
        _models.push_back(model);
        _renderMutex.unlock();

        emit appearedSmthToDraw();

        renderNext();
    }

    void ModelRenderer::render() {
        // if no models present -> no need to render
        if (!_models.size()) {
            return;
        }

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        _viewPorts.resize(_surfaceSize);

        ViewPort viewPort;
        ViewPortRect boundingRect;

        ViewPortsIterator itV (_viewPorts);
        QVectorIterator<Model::AbstractModel *> itM (_models);

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
                itM.next()->drawModel(viewPort);
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

        activateContext();

        if (_textures.keys().size()) {
            qDeleteAll(_textures.keys().begin(), _textures.keys().end());
            _textures.clear();
        }

        qDeleteAll(_models.begin(), _models.end());
        _models.clear();

        qDeleteAll(_materials.begin(), _materials.end());
        _materials.clear();

        qDeleteAll(_lightSources.begin(), _lightSources.end());
        _lightSources.clear();
    }
}
