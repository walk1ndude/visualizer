#include <QtGui/QColor>
#include <QtGui/QScreen>
#include <QtGui/QOpenGLPixelTransferOptions>

#include <opencv2/highgui/highgui.hpp>

#include "glviewer.h"

GLviewer::GLviewer(const std::vector<cv::Mat *> & ctImages) :
    _program(0),
    _rBottom((float) 0.1),
    _rTop((float) 0.8),
    _textureCV3D(QOpenGLTexture::Target3D),
    _ctImages(ctImages) {

    _matrixStack.identity(QVector3D(0.0, 0.0, -4.0));
    _matrixStack.ortho(-2.0, 2.0, -2.0, 2.0, 0.001, 100.0);
    _matrixStack.lookAt(QVector3D(0.0, 0.0, -4.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

    fetchHud();
}

GLviewer::~GLviewer() {
    _textureCV3D.release();
}

void GLviewer::fetchHud() {
    _hud = new Hud(this);

    QObject::connect(_hud, SIGNAL(rBottomChanged(qreal)), this, SLOT(updateRBottom(qreal)));
    QObject::connect(_hud, SIGNAL(rTopChanged(qreal)), this, SLOT(updateRTop(qreal)));

    QObject::connect(_hud, SIGNAL(xRotChanged(qreal)), this, SLOT(updateXRot(qreal)));
    QObject::connect(_hud, SIGNAL(yRotChanged(qreal)), this, SLOT(updateYRot(qreal)));
    QObject::connect(_hud, SIGNAL(zRotChanged(qreal)), this, SLOT(updateZRot(qreal)));
    QObject::connect(_hud, SIGNAL(distChanged(qreal)), this, SLOT(updateDist(qreal)));

    _hud->show();
}

void GLviewer::initialize() {
    _program = new QOpenGLShaderProgram(this);
    _program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":shaders/vertex.glsl");
    _program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":shaders/fragment.glsl");
    _program->link();

    _shaderModelView = _program->uniformLocation("modelView");
    _shaderProjection = _program->uniformLocation("projection");
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

    _hud->resize(width() * retinaScale, 0.2 * height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    _program->bind();

    _program->setUniformValue(_shaderModelView, _matrixStack.modelView());
    _program->setUniformValue(_shaderProjection, _matrixStack.projection());
    _program->setUniformValue(_shaderTexSample, 0);
    _program->setUniformValue(_shaderRBottom, (GLfloat) _rBottom);
    _program->setUniformValue(_shaderRTop, (GLfloat) _rTop);

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

    float xRot = 0.0;
    float yRot = 0.0;
    float zRot = 0.0;

    switch (key) {
    case Qt::Key_Up:
        xRot = -10.0;
        break;

    case Qt::Key_Down:
        xRot = 10.0;
        break;

    case Qt::Key_Left:
        yRot = -10.0;
        break;

    case Qt::Key_Right:
        yRot = 10.0;
        break;

    default:
        break;
    }

    _matrixStack.rotate(QVector3D(xRot, yRot, zRot));
    renderNow();
}

void GLviewer::updateRBottom(qreal rBottom) {
    _rBottom = std::min(rBottom, _rTop);
    _rTop = std::max(rBottom, _rTop);
    renderNow();
}

void GLviewer::updateRTop(qreal rTop) {
    _rTop = std::max(rTop, _rBottom);
    _rBottom = std::min(rTop, _rBottom);
    renderNow();
}

void GLviewer::updateXRot(qreal xRot) {
   _matrixStack.rotate(QVector3D(xRot, 0.0, 0.0));
   renderNow();
}

void GLviewer::updateYRot(qreal yRot) {
   _matrixStack.rotate(QVector3D(0.0, yRot, 0.0));
   renderNow();
}

void GLviewer::updateZRot(qreal zRot) {
   _matrixStack.rotate(QVector3D(0.0, 0.0, zRot));
   renderNow();
}

void GLviewer::updateDist(qreal dist) {
    _matrixStack.translate(QVector3D(0.0, 0.0, dist));
    renderNow();
}
