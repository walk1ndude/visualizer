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
    Q_PROPERTY(qreal zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

    Q_PROPERTY(QVector2D sRange READ sRange WRITE setSRange NOTIFY sRangeChanged)
    Q_PROPERTY(QVector2D tRange READ tRange WRITE setTRange NOTIFY tRangeChanged)
    Q_PROPERTY(QVector2D pRange READ pRange WRITE setPRange NOTIFY pRangeChanged)

    Q_PROPERTY(QVector2D huRange READ huRange WRITE sethuRange NOTIFY huRangeChanged)

    Q_PROPERTY(int minHU READ minHU WRITE setMinHU NOTIFY minHUChanged)
    Q_PROPERTY(int maxHU READ maxHU WRITE setMaxHU NOTIFY maxHUChanged)

public:
    explicit SliceViewer();
    virtual ~SliceViewer();

    QVector3D rotation();
    void setRotation(const QVector3D & rotation);

    qreal zoomFactor();
    void setZoomFactor(const qreal & zoomFactor);

    QVector2D sRange();
    void setSRange(const QVector2D & sRange);

    QVector2D tRange();
    void setTRange(const QVector2D & tRange);

    QVector2D pRange();
    void setPRange(const QVector2D & pRange);

    QVector2D huRange();
    void sethuRange(const QVector2D & huRange);

    int minHU();
    void setMinHU(const int & minHU);

    int maxHU();
    void setMaxHU(const int & maxHU);

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
    int _shaderStep;

    int _shaderNormalMatrix;

    int _shaderSRange;
    int _shaderTRange;
    int _shaderPRange;

    int _shaderTexHead;

    int _shaderLightPosition;
    int _shaderLightColor;
    int _shaderLightAmbientIntensity;

    QVector3D _step;

    QVector2D _sRange;
    QVector2D _tRange;
    QVector2D _pRange;

    QVector2D _huRange;

    qreal _zoomFactor;

    int64_t _minHU;
    int64_t _maxHU;

    bool _slicesReady;

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

    void initializeTexture(QOpenGLTexture ** texture, QSharedPointer<uchar> & textureData,
                           const QOpenGLTexture::TextureFormat & textureFormat,
                           const QOpenGLTexture::PixelFormat & pixelFormat,
                           const QOpenGLTexture::PixelType & pixelType,
                           const QOpenGLPixelTransferOptions * pixelOptions);

signals:
    void rotationChanged();
    void zoomFactorChanged();

    void sRangeChanged();
    void tRangeChanged();
    void pRangeChanged();

    void huRangeChanged();

    void minHUChanged(const int & minHU);
    void maxHUChanged(const int & maxHU);

public slots:
    void drawSlices(QSharedPointer<uchar> mergedData,
                    const std::vector<float> & scaling = std::vector<float>(),
                    const std::vector<size_t> & size = std::vector<size_t>(),
                    const int & alignment = 0, const size_t & rowLength = 0,
                    const std::vector<int> & huInterval = std::vector<int>());

    virtual void cleanup();
};

#endif // SLICEVIEWER_H
