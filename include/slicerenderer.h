#ifndef SLICERENDERER_H
#define SLICERENDERER_H

#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLPixelTransferOptions>

#include <QtCore/QSharedPointer>

#include "renderthread.h"
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

class SliceRenderer : public RenderThread {
    Q_OBJECT

public:
    explicit SliceRenderer(QOpenGLContext * context, const QSize & size);
    virtual ~SliceRenderer();

protected:
    virtual void initialize();
    virtual void updateTextures();
    virtual void render();

private:
    QOpenGLShaderProgram * _program;

    ViewPorts _viewPorts;

    QVector3D _step;

    QVector2D _sRange;
    QVector2D _tRange;
    QVector2D _pRange;

    qreal _zoomFactor;

    int _shaderModel;
    int _shaderView;
    int _shaderProjection;
    int _shaderScale;
    int _shaderStep;

    int _shaderNormalMatrix;

    int _shaderSRange;
    int _shaderTRange;
    int _shaderPRange;

    int _shaderTexHead;

    int _shaderLightPosition;
    int _shaderLightColor;
    int _shaderLightAmbientIntensity;

    QOpenGLTexture * _textureHead;
    QOpenGLPixelTransferOptions _pixelOptionsHead;

    GLfloat _ambientIntensity;

    std::vector<float>_scaling;
    std::vector<size_t>_size;

    LightSource _lightSource;

    HeadModel _headModel;

    QSharedPointer<uchar>_mergedData;

    GPU_Driver _gpu_driver;

    void initializeViewPorts();
    void cleanUp();

    void updateTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                           const QOpenGLTexture::TextureFormat & textureFormat,
                           const QOpenGLTexture::PixelFormat & pixelFormat,
                           const QOpenGLTexture::PixelType & pixelType,
                           const QOpenGLPixelTransferOptions * pixelOptions);

public slots:
    void drawSlices(QSharedPointer<uchar> mergedData,
                    const std::vector<float> & scaling = std::vector<float>(),
                    const std::vector<size_t> & size = std::vector<size_t>(),
                    const int & alignment = 0, const size_t & rowLength = 0);

    void setTakeShot(const bool & takeShot);

    void setRotation(const QVector3D & rotation);

    void setZoomFactor(const qreal & zoomFactor);

    void setSRange(const QVector2D & sRange);
    void setTRange(const QVector2D & tRange);
    void setPRange(const QVector2D & pRange);
};

#endif // SLICERENDERER_H
