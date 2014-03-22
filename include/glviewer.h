#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>

#include <opencv2/core/core.hpp>

#include "openglwindow.h"
#include "geometryengine.h"
#include "matrixstack.h"

class GLviewer : public OpenGLWindow {
    Q_OBJECT
    
public:
    explicit GLviewer(const std::vector<cv::Mat *> & ctImages);

    ~GLviewer();

    void initialize();
    void render();

protected:
    void initTextures();

    void paintGL();

    void resizeGL(int width, int height);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent * event);

private:
    QOpenGLShaderProgram * _program;

    MatrixStack _matrixStack;

    int _shaderMatrix;
    int _shaderTexSample;
    int _shaderRBottom;
    int _shaderRTop;

    float _rBottom;
    float _rTop;

    int _count;

    QPoint _lastMousePosition;

    GeometryEngine _geometryEngine;

    QOpenGLTexture _textureCV3D;

    std::vector<cv::Mat*> _ctImages;
signals:

public slots:

};

#endif // GLVIEWER_H