#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QKeyEvent>

#include "openglwindow.h"
#include "hud.h"
#include "glheadmodel.h"
#include "matrixstack.h"

class GLviewer : public OpenGLWindow {
    Q_OBJECT
    
public:
    explicit GLviewer(const QSurfaceFormat & surfaceFormat, QWindow * parent = 0);
    ~GLviewer();

    void initialize();
    void render();

protected:
    void keyPressEvent(QKeyEvent * event);

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

    int _alignment;

    size_t _rowLength;

    QOpenGLTexture _textureCV3D;

    qreal _rBottom;
    qreal _rTop;

    std::vector<float>_scaling;
    std::vector<size_t>_size;

    QPoint _lastMousePosition;

    GLHeadModel _glHeadModel;

    const uchar * _mergedData;

    Hud * _hud;

    void fetchHud();
    void initTextures();
signals:

public slots:
    void drawSlices(const uchar * mergedData, const std::vector<float> & scaling,
                    const std::vector<size_t> & size, const int & alignment, const size_t & rowLength);

    void updateRBottom(qreal rBottom);
    void updateRTop(qreal rTop);

    void updateAngle(qreal xRot, qreal yRot, qreal zRot);
    void updateZoomZ(qreal zoomZ);
};

#endif // GLVIEWER_H
