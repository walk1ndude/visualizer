#ifndef SLICEVIEWER_H
#define SLICEVIEWER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include "openglitem.h"
#include "headmodel.h"
#include "viewports.h"

typedef enum _GPU_Driver {
    NVidia_binary,
    AMD_binary,
    AMD_opensource,
    NVidia_opensourse,
    Intel_opensource
} GPU_Driver;

typedef struct _LightSource {
    QVector4D position;
    QVector4D color;
    float ambientIntensity;
} LightSource;

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

    ViewPorts _viewPorts;

    int _shaderModel;
    int _shaderView;
    int _shaderProjection;
    int _shaderScale;
    int _shaderNormalMatrix;

    int _shaderSRange;
    int _shaderTRange;
    int _shaderPRange;

    int _shaderTexHead;
    int _shaderTexGradient;

    int _shaderLightPosition;
    int _shaderLightColor;
    int _shaderLightAmbientIntensity;

    QVector2D _sRange;
    QVector2D _tRange;
    QVector2D _pRange;

    bool _slicesReady;

    QOpenGLTexture * _textureHead;
    QOpenGLTexture * _textureGradient;

    QOpenGLPixelTransferOptions _pixelOptionsHead;
    QOpenGLPixelTransferOptions _pixelOptionsGradient;

    GLfloat _ambientIntensity;

    std::vector<float>_scaling;
    std::vector<size_t>_size;

    LightSource _lightSource;

    HeadModel _headModel;

    uchar * _mergedData;
    uchar * _gradientData;

    GPU_Driver _gpu_driver;

    void initializeViewPorts();

    void initializeTexture(QOpenGLTexture ** texture, uchar ** data,
                           const QOpenGLTexture::TextureFormat & textureFormat,
                           const QOpenGLTexture::PixelFormat & pixelFormat,
                           const QOpenGLPixelTransferOptions * pixelOptions);

public slots:
    void drawSlices(uchar * mergedData, uchar * gradientData,
                    const std::vector<float> & scaling = std::vector<float>(),
                    const std::vector<size_t> & size = std::vector<size_t>(),
                    const int & alignment = 0, const size_t & rowLength = 0,
                    const int & alignmentGradient = 0, const size_t & rowLengthGradient = 0);

    void updateAmbientIntensity(qreal ambientIntensity);

    void updateAngle(qreal xRot, qreal yRot, qreal zRot);
    void updateZoom(qreal factor);

    void updateSRange(QVector2D sRange);
    void updateTRange(QVector2D tRange);
    void updatePRange(QVector2D pRange);

    virtual void cleanup();
};

#endif // SLICEVIEWER_H
