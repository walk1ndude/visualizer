#include <QtGui/QColor>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLPixelTransferOptions>

#include <QtQuick/QQuickWindow>

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
    _slicesReady(false),
    _textureCV3D(0),
    _ambientIntensity((GLfloat) 3.9),
    _mergedData(0),
    _gpu_driver(NVidia_binary) {

    _lightSource.color = QVector4D(0.5, 0.5, 0.0, 1.0);
    _lightSource.position = QVector4D(10.0, 10.0, -10.0, 0.0);

}

SliceViewer::~SliceViewer() {

}

void SliceViewer::calcViewPorts() {
    int halfWidth = width() / 2;
    int halfHeight = height() / 2;

    _viewPorts[0] = QRect(halfWidth + 5, 0, halfWidth - 5, halfHeight - 5);
    _viewPorts[1] = QRect(0, 0, halfWidth - 5, halfHeight - 5);
    _viewPorts[2] = QRect(0, halfHeight + 5, halfWidth - 5, halfHeight - 5);
    _viewPorts[3] = QRect(halfWidth + 5, halfHeight + 5, halfWidth - 5, halfHeight - 5);
}

void SliceViewer::drawSlices(uchar * mergedData, const std::vector<float> & scaling, const std::vector<size_t> & size,
                          const int & alignment, const size_t & rowLength) {

    _mergedData = mergedData;

    if (_slicesReady) {
        _isTextureUpdated = false;
        update();
        return;
    }

    cleanup();

    _scaling = scaling;
    _size = size;

    _alignment = alignment;
    _rowLength = rowLength;

    _matrices.resize(4);

    for (int i = 0; i != 4; ++ i) {
        _matrices[i].identity();
        if (i == 0) { //perspective
            _matrices[i].perspective(60.0, 1.0, 0.1, 10.0);
            _matrices[i].lookAt(QVector3D(0.0, 0.0, 2.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
        }
        else {
            _matrices[i].ortho(-0.9, 0.9, -0.9, 0.9, 0.01, 10.0);
            _matrices[i].lookAt(QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));
        }
        _matrices[i].scale(QVector3D(_scaling[0], _scaling[1], _scaling[2]));
    }

    _matrices[0].rotate(QVector3D(-90.0, 0.0, 0.0));
    _matrices[2].rotate(QVector3D(-90.0, 0.0, 0.0));
    _matrices[3].rotate(QVector3D(-90.0, -90.0, 0.0));

    _viewPorts.resize(4);

    _slicesReady = true;
    _needsInitialize = true;

    update();
}

void SliceViewer::initialize() {

    if (!_slicesReady) {
        return;
    }

    gpu_profiling(_gpu_driver, "initialization begin");

    _program = new QOpenGLShaderProgram;
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/sliceVertex.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/sliceFragment.glsl");
    _program->link();

    _shaderModel = _program->uniformLocation("model");
    _shaderView = _program->uniformLocation("view");
    _shaderProjection = _program->uniformLocation("projection");
    _shaderScale = _program->uniformLocation("scale");
    _shaderNormalMatrix = _program->uniformLocation("normalMatrix");
    _shaderAmbientIntensity = _program->uniformLocation("ambientIntensity");
    _shaderLightPosition = _program->uniformLocation("lightPosition");
    _shaderLightColor = _program->uniformLocation("lightColor");
    _shaderTexSample = _program->uniformLocation("texSample");

    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initializeTextures();

    _headModel.init(_program, _size[2]);

    gpu_profiling(_gpu_driver, "initialization end");
}

void SliceViewer::render() {
    if (!_program) {
        return;
    }

    calcViewPorts();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    _textureCV3D->bind();

    _program->bind();

    for (int i = 0; i != 4; ++ i) {
        glViewport(_viewPorts[i].x(), _viewPorts[i].y(), _viewPorts[i].width(), _viewPorts[i].height());

        _program->setUniformValue(_shaderModel, _matrices[i].model());
        _program->setUniformValue(_shaderView, _matrices[i].view());
        _program->setUniformValue(_shaderProjection, _matrices[i].projection());
        _program->setUniformValue(_shaderScale, _matrices[i].scaleM());
        _program->setUniformValue(_shaderNormalMatrix, _matrices[i].normalM());
        _program->setUniformValue(_shaderAmbientIntensity, _ambientIntensity);
        _program->setUniformValue(_shaderLightPosition, _lightSource.position);
        _program->setUniformValue(_shaderLightColor, _lightSource.color);
        _program->setUniformValue(_shaderTexSample, 0);

        _headModel.drawModel(_program);
    }

    _program->release();

    gpu_profiling(_gpu_driver, "actual drawing");

    _textureCV3D->release();
}

void SliceViewer::cleanup() {
    if (_program) {
        delete _program;

        if (_textureCV3D->isStorageAllocated()) {
            _textureCV3D->destroy();
        }

        _headModel.destroyModel();
    }
}

void SliceViewer::initializeTextures() {

    if (!_textureCV3D) {
        _textureCV3D = new QOpenGLTexture(QOpenGLTexture::Target3D);

        _textureCV3D->create();
        _textureCV3D->setSize(_size[0], _size[1], _size[2]);
        _textureCV3D->setFormat(QOpenGLTexture::R8_UNorm);

        _textureCV3D->allocateStorage();

        _textureCV3D->setSwizzleMask(QOpenGLTexture::RedValue, QOpenGLTexture::RedValue, QOpenGLTexture::RedValue, QOpenGLTexture::RedValue);

        _textureCV3D->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
        _textureCV3D->setMagnificationFilter(QOpenGLTexture::Linear);
        _textureCV3D->setWrapMode(QOpenGLTexture::ClampToBorder);
    }

    QOpenGLPixelTransferOptions pixelOptions;
    pixelOptions.setAlignment(_alignment);
    pixelOptions.setRowLength(_rowLength);

    _textureCV3D->setData(QOpenGLTexture::Red, QOpenGLTexture::UInt8, (void *) _mergedData, &pixelOptions);

    _textureCV3D->generateMipMaps();

    delete [] _mergedData;
    _mergedData = 0;
}

void SliceViewer::updateAngle(qreal xRot, qreal yRot, qreal zRot) {
    _matrices[0].rotate(QVector3D(xRot + 90.0, yRot, zRot)); // perspective
    _matrices[1].rotate(QVector3D(0.0, 0.0, zRot));
    _matrices[2].rotate(QVector3D(xRot + 90.0, 0.0, 0.0));
    _matrices[3].rotate(QVector3D(90.0, yRot + 90.0, 0.0));

    update();
}

void SliceViewer::updateZoom(qreal factor) {
    for (int i = 0; i!= 4; ++ i) {
        _matrices[i].zoom(factor);
    }
    update();
}

void SliceViewer::updateAmbientIntensity(qreal ambientIntensity) {
    _ambientIntensity = ambientIntensity;
    update();
}
