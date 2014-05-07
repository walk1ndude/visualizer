#include <QtQuick/QQuickWindow>

#include <iostream>

#include "sliceviewer.h"

void gpu_profiling(const GPU_Driver & gpu_driver, const QString & debugMessage) {
   GLint nCurAvailMemoryInKB = 0;
   GLint nTotalMemoryInKB = 0;

   switch (gpu_driver) {
   case NVidia_binary :
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

       glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, & nCurAvailMemoryInKB);
       glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, & nTotalMemoryInKB);

#undef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
#undef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
       break;
   case NVidia_opensourse :
   case AMD_opensource :
   case AMD_binary :
   case Intel_opensource :
       break;
   }

   qDebug() << debugMessage
            << "Total: " << nTotalMemoryInKB / 1024
            << "MiB, Used:" << (nTotalMemoryInKB - nCurAvailMemoryInKB) / 1024
            << "MiB, Free:" << nCurAvailMemoryInKB / 1024 << "MiB";
}

SliceViewer::SliceViewer() :
    OpenGLItem(),
    _program(0),
    _sRange(QVector2D(0.0, 1.0)),
    _tRange(QVector2D(0.0, 1.0)),
    _pRange(QVector2D(0.0, 1.0)),
    _zoomFactor(2.0),
    _slicesReady(false),
    _textureHead(0),
    _ambientIntensity((GLfloat) 3.9),
    _mergedData(0),
    _gpu_driver(NVidia_binary) {

    _lightSource.color = QVector4D(1.0, 1.0, 1.0, 1.0);
    _lightSource.position = QVector4D(10.0, 10.0, -10.0, 0.0);
    _lightSource.ambientIntensity = 4.3;

}

SliceViewer::~SliceViewer() {

}

QVector3D SliceViewer::rotation() {
    return _rotation;
}

void SliceViewer::setRotation(const QVector3D & rotation) {
    _rotation = rotation;

    _viewPorts.rotate(_rotation.x(), _rotation.y(), _rotation.z());
    update();
}

qreal SliceViewer::zoomFactor() {
    return _zoomFactor;
}

void SliceViewer::setZoomFactor(const qreal & zoomFactor) {
    _viewPorts.zoom(zoomFactor);
    update();
}

QVector2D SliceViewer::sRange() {
    return _sRange;
}

void SliceViewer::setSRange(const QVector2D & sRange) {
    _sRange = sRange;
    update();
}

QVector2D SliceViewer::tRange() {
    return _tRange;
}

void SliceViewer::setTRange(const QVector2D & tRange) {
    _tRange = tRange;
    update();
}

QVector2D SliceViewer::pRange() {
    return _pRange;
}

void SliceViewer::setPRange(const QVector2D & pRange) {
    _pRange = pRange;
    update();
}

QVector2D SliceViewer::huRange() {
    return _huRange;
}

void SliceViewer::sethuRange(const QVector2D & huRange) {
    _huRange = huRange;
    emit huRangeChanged();
}

int SliceViewer::minHU() {
    return _minHU;
}

void SliceViewer::setMinHU(const int & minHU) {
    _minHU = minHU;
    emit minHUChanged(_minHU);
}

int SliceViewer::maxHU() {
    return _maxHU;
}

void SliceViewer::setMaxHU(const int & maxHU) {
    _maxHU = maxHU;
    emit maxHUChanged(_maxHU);
}

void SliceViewer::initializeViewPorts() {
    QVector<QPair<QRectF, ViewPort::ProjectionType> > viewPorts;

    viewPorts.push_back(
                QPair<QRectF, ViewPort::ProjectionType>(
                    QRectF(0.5, 0, 0.5, 0.5), ViewPort::PERSPECTIVE)
                );

    viewPorts.push_back(
                QPair<QRectF, ViewPort::ProjectionType>(
                    QRectF(0, 0, 0.5, 0.5), ViewPort::BOTTOM)
                );

    viewPorts.push_back(
                QPair<QRectF, ViewPort::ProjectionType>(
                    QRectF(0, 0.5, 0.5, 0.5), ViewPort::FRONT)
                );

    viewPorts.push_back(
                QPair<QRectF, ViewPort::ProjectionType>(
                    QRectF(0.5, 0.5, 0.5, 0.5), ViewPort::LEFT)
                );

    _viewPorts.setViewPorts(viewPorts, window()->size());
}

void SliceViewer::drawSlices(QSharedPointer<uchar> mergedData,
                             const std::vector<float> & scaling, const std::vector<size_t> & size,
                             const int & alignment, const size_t & rowLength,
                             const std::vector<int> & huRange) {

    _mergedData = mergedData;

    if (_slicesReady) {
        _isTextureUpdated = false;
        update();
        return;
    }

    cleanup();

    _scaling = scaling;
    _size = size;

    _step = QVector3D(1.0 / _size[0], 1.0 / _size[1], 1.0 / _size[2]);

    if (rowLength) {
        _pixelOptionsHead.setAlignment(alignment);
        _pixelOptionsHead.setRowLength(rowLength);
    }

    _viewPorts.scale(QVector3D(_scaling[0], _scaling[1], _scaling[2]));

    _slicesReady = true;
    _needsInitialize = true;

    if (huRange.size()) {
        _huRange = QVector2D(huRange[0], huRange[1]);
        emit huRangeChanged();
    }

    update();
}

void SliceViewer::initialize() {

    if (!_slicesReady) {
        return;
    }

    //gpu_profiling(_gpu_driver, "initialization begin");

    _program = new QOpenGLShaderProgram;
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/sliceVertex.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/sliceFragment.glsl");
    _program->link();

    _shaderModel = _program->uniformLocation("model");
    _shaderView = _program->uniformLocation("view");
    _shaderProjection = _program->uniformLocation("projection");
    _shaderScale = _program->uniformLocation("scale");
    _shaderStep = _program->uniformLocation("stepSlices");

    _shaderNormalMatrix = _program->uniformLocation("normalMatrix");

    _shaderLightPosition = _program->uniformLocation("light.position");
    _shaderLightColor = _program->uniformLocation("light.color");
    _shaderLightAmbientIntensity = _program->uniformLocation("light.ambientIntensity");

    _shaderSRange = _program->uniformLocation("ranges.sRange");
    _shaderTRange = _program->uniformLocation("ranges.tRange");
    _shaderPRange = _program->uniformLocation("ranges.pRange");

    _shaderTexHead = _program->uniformLocation("texHead");

    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    initializeTextures();

    _headModel.init(_program, _size[2]);

    //gpu_profiling(_gpu_driver, "initialization end");
}

void SliceViewer::render() {
    if (!_program) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    QSize sizeW = window()->size();
    sizeW.setHeight(sizeW.height() * window()->devicePixelRatio());
    sizeW.setWidth(sizeW.width() * window()->devicePixelRatio());

    _viewPorts.resize(sizeW);

    _textureHead->bind();

    _program->bind();

    QRectF boundingRect;

    for (int i = 0; i != 4; ++ i) {
        boundingRect = _viewPorts[i].boundingRect();

        if (_viewPorts[i].projectionType() == ViewPort::PERSPECTIVE) {
            _screenSaveRect = boundingRect;
        }

        glViewport(boundingRect.x(), boundingRect.y(), boundingRect.width(), boundingRect.height());

        _program->setUniformValue(_shaderModel, _viewPorts[i].model());
        _program->setUniformValue(_shaderView, _viewPorts[i].view());
        _program->setUniformValue(_shaderProjection, _viewPorts[i].projection());
        _program->setUniformValue(_shaderScale, _viewPorts[i].scaleM());
        _program->setUniformValue(_shaderStep, _step);

        _program->setUniformValue(_shaderNormalMatrix, _viewPorts[i].normalM());

        _program->setUniformValue(_shaderLightPosition, _lightSource.position);
        _program->setUniformValue(_shaderLightColor, _lightSource.color);
        _program->setUniformValue(_shaderLightAmbientIntensity, _lightSource.ambientIntensity);

        _program->setUniformValue(_shaderTexHead, 0);

        _program->setUniformValue(_shaderSRange, _sRange);
        _program->setUniformValue(_shaderTRange, _tRange);
        _program->setUniformValue(_shaderPRange, _pRange);

        _headModel.drawModel(_program);
    }

    _program->release();

    //gpu_profiling(_gpu_driver, "actual drawing");

    _textureHead->release();
}

void SliceViewer::cleanup() {
    if (_program) {
        delete _program;

        if (_textureHead->isStorageAllocated()) {
            _textureHead->destroy();
        }

        _headModel.destroyModel();
    }
}

void SliceViewer::initializeTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                                    const QOpenGLTexture::TextureFormat & textureFormat,
                                    const QOpenGLTexture::PixelFormat & pixelFormat,
                                    const QOpenGLTexture::PixelType & pixelType,
                                    const QOpenGLPixelTransferOptions * pixelOptions) {


    QOpenGLTexture * tex = *texture;
    if (!tex) {
        tex = new QOpenGLTexture(QOpenGLTexture::Target3D);

        tex->create();

        tex->setSize(_size[0], _size[1], _size[2]);
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

void SliceViewer::initializeTextures() {
    initializeTexture(&_textureHead, _mergedData, QOpenGLTexture::R16_UNorm,
                      QOpenGLTexture::Red, QOpenGLTexture::UInt16, &_pixelOptionsHead);
}
