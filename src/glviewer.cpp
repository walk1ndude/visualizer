#include <QtGui/QColor>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLPixelTransferOptions>

#include <opencv2/highgui/highgui.hpp>

#include "glviewer.h"

GLviewer::GLviewer(const std::vector<cv::Mat *> & ctImages) :
    _program(0),
    _rBottom((float) 0.22),
    _rTop((float) 0.3),
    _textureCV3D(QOpenGLTexture::Target3D),
    _ctImages(ctImages) {

    ViewPortParams viewPortParams;

    viewPortParams.left = -2.0;
    viewPortParams.right = 2.0;
    viewPortParams.bottom = -2.0;
    viewPortParams.top = 2.0;
    viewPortParams.nearVal = 0.001;
    viewPortParams.farVal = 100.0;

    _matrixStack.initialize(viewPortParams);
}

GLviewer::~GLviewer() {
    _textureCV3D.release();
}

void GLviewer::initialize() {
    _program = new QOpenGLShaderProgram(this);
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/vertex.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/fragment.glsl");
    _program->link();

    _shaderMatrix = _program->uniformLocation("mvpMatrix");
    _shaderTexSample = _program->uniformLocation("texSample");
    _shaderRBottom = _program->uniformLocation("rBottom");
    _shaderRTop = _program->uniformLocation("rTop");

    _count = _ctImages.size();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHTING);

    initTextures();

    _geometryEngine.init(_program, _count);
}

void GLviewer::render() {
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    _program->bind();

    _program->setUniformValue(_shaderMatrix, _matrixStack.mvpMatrix());
    _program->setUniformValue(_shaderTexSample, 0);
    _program->setUniformValue(_shaderRBottom, _rBottom);
    _program->setUniformValue(_shaderRTop, _rTop);

    _textureCV3D.bind();

    _geometryEngine.drawModel(_program);

    _program->release();
}

void GLviewer::initTextures() {

    cv::Mat image(*(_ctImages[0]));

    QOpenGLPixelTransferOptions pixelOptions;
    pixelOptions.setAlignment((image.step & 3) ? 1 : 4);
    pixelOptions.setRowLength(image.step1());

    int byteSizeMat = _ctImages[0]->elemSize() * _ctImages[0]->total();
    int byteSizeAll = byteSizeMat * _count;

    uchar * data = new uchar[byteSizeAll];

    for (int i = 0; i != _count; ++ i) {
        cv::flip(*(_ctImages[i]), image, 0);

        memcpy(data + byteSizeMat * i, image.data, byteSizeMat);
    }

    _textureCV3D.setSize(image.cols, image.rows, _count);
    _textureCV3D.setFormat(QOpenGLTexture::R16_UNorm);
    _textureCV3D.allocateStorage();

    _textureCV3D.setData(QOpenGLTexture::Red, QOpenGLTexture::UInt16, (void *) data, &pixelOptions);

    _textureCV3D.setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    _textureCV3D.setMagnificationFilter(QOpenGLTexture::Linear);
    _textureCV3D.setWrapMode(QOpenGLTexture::ClampToBorder);

    _textureCV3D.generateMipMaps();
}

void GLviewer::mousePressEvent(QMouseEvent * event) {
    _lastMousePosition = event->pos();

    event->accept();
}

void GLviewer::mouseMoveEvent(QMouseEvent * event) {
    //int dX = event->x() - _lastMousePosition.x();
    //int dY = event->y() - _lastMousePosition.y();

    _lastMousePosition = event->pos();

    event->accept();
}

void GLviewer::wheelEvent(QWheelEvent * event) {
    //int d = event->delta();

    event->accept();
}

void GLviewer::keyPressEvent(QKeyEvent * event) {
    int key = event->key();

    float dX = 0.0;
    float dY = 0.0;
    float dZ = 0.0;

    switch (key) {
    case Qt::Key_Up:
        dX = -10.0;
        break;

    case Qt::Key_Down:
        dX = 10.0;
        break;

    case Qt::Key_Left:
        dY = -10.0;
        break;

    case Qt::Key_Right:
        dY = 10.0;
        break;

    default:
        break;
    }

    _matrixStack.setAngles(QVector3D(dX, dY, dZ));
    renderNow();
}
