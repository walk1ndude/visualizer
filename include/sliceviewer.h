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
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    
public:
    explicit SliceViewer();
    virtual ~SliceViewer();

    QVector3D rotation();
    void setRotation(const QVector3D & rotation);

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

    QVector3D _rotation;

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

    QSharedPointer<uchar>_mergedData;
    QSharedPointer<uchar>_gradientData;

    GPU_Driver _gpu_driver;

    void initializeViewPorts();

    void initializeTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                           const QOpenGLTexture::TextureFormat & textureFormat,
                           const QOpenGLTexture::PixelFormat & pixelFormat,
                           const QOpenGLPixelTransferOptions * pixelOptions);

signals:
    void rotationChanged();

public slots:
    void drawSlices(QSharedPointer<uchar> mergedData, QSharedPointer<uchar> gradientData,
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
