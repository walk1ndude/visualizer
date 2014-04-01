#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QtGui/QOpenGLTexture>

#include <opencv2/core/core.hpp>

#include "openglwindow.h"
#include "hud.h"
#include "geometryengine.h"
#include "matrixstack.h"

class GLviewer : public OpenGLWindow {
    Q_OBJECT
    
public:
    explicit GLviewer(const std::vector<cv::Mat *> & ctImages, const std::vector<float> & imageSpacings);

    ~GLviewer();

    void initialize();
    void render();

protected:
    void initTextures();

    void paintGL();

    void resizeGL(int width, int height);

private:
    QOpenGLShaderProgram * _program;

    MatrixStack _matrixStack;

    int _shaderModel;
    int _shaderView;
    int _shaderProjection;
    int _shaderScale;
    int _shaderTexSample;
    int _shaderRBottom;
    int _shaderRTop;

    qreal _rBottom;
    qreal _rTop;

    int _count;

    std::vector<float>_imageSpacings;

    QPoint _lastMousePosition;

    GeometryEngine _geometryEngine;

    QOpenGLTexture _textureCV3D;

    std::vector<cv::Mat*> _ctImages;

    Hud * _hud;

    void fetchHud();
signals:

public slots:
    void updateRBottom(qreal rBottom);
    void updateRTop(qreal rTop);

    void updateAngle(qreal xRot, qreal yRot, qreal zRot);
    void updateDist(qreal dist);
};

#endif // GLVIEWER_H
