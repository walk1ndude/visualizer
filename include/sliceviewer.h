#ifndef GLVIEWER_H
#define GLVIEWER_H

#include <QtGui/QOpenGLTexture>

#include "openglitem.h"
#include "headmodel.h"
#include "matrixstack.h"

typedef enum _GPU_Driver {
    NVidia_binary,
    AMD_binary,
    AMD_opensource,
    NVidia_opensourse,
    Intel_opensource
}GPU_Driver;

void gpu_profiling(const GPU_Driver & gpu_driver, const QString & debugMessage = "");

class SliceViewer : public OpenGLItem {
    Q_OBJECT
    
public:
    explicit SliceViewer();
    virtual ~SliceViewer();

protected:
    virtual void initialize();
    virtual void initializeTextures();
    virtual void render();

private:
    QOpenGLShaderProgram * _program;

    QVector<MatrixStack>_matrices;

    QVector<QRect>_viewPorts;

    int _shaderModel;
    int _shaderView;
    int _shaderProjection;
    int _shaderScale;
    int _shaderNormalMatrix;
    int _shaderLightPos;
    int _shaderAmbientIntensity;
    int _shaderTexSample;

    int _alignment;

    bool _slicesReady;

    size_t _rowLength;

    QOpenGLTexture * _textureCV3D;

    GLfloat _ambientIntensity;

    std::vector<float>_scaling;
    std::vector<size_t>_size;

    QVector3D _lightPos;

    QPoint _lastMousePosition;

    HeadModel _headModel;

    uchar * _mergedData;

    GPU_Driver _gpu_driver;

    void calcViewPorts();

public slots:
    void drawSlices(uchar * mergedData, const std::vector<float> & scaling,
                    const std::vector<size_t> & size, const int & alignment, const size_t & rowLength);

    void updateAmbientIntensity(qreal ambientIntensity);

    void updateAngle(qreal xRot, qreal yRot, qreal zRot);
    void updateZoomZ(qreal zoomZ);

    virtual void cleanup();
};

#endif // GLVIEWER_H
