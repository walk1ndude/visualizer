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

    Q_PROPERTY(int minValue READ minValue WRITE setMinValue NOTIFY minValueChanged)
    Q_PROPERTY(int maxValue READ maxValue WRITE setMaxValue NOTIFY maxValueChanged)

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

    int minValue();
    void setMinValue(const int & minValue);

    int maxValue();
    void setMaxValue(const int & maxValue);

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

    qreal _zoomFactor;

    int _minValue;
    int _maxValue;

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
    void zoomFactorChanged();

    void sRangeChanged();
    void tRangeChanged();
    void pRangeChanged();

    void minValueChanged(const int & minValue);
    void maxValueChanged(const int & maxValue);

public slots:
    void drawSlices(QSharedPointer<uchar> mergedData, QSharedPointer<uchar> gradientData,
                    const std::vector<float> & scaling = std::vector<float>(),
                    const std::vector<size_t> & size = std::vector<size_t>(),
                    const int & alignment = 0, const size_t & rowLength = 0,
                    const int & alignmentGradient = 0, const size_t & rowLengthGradient = 0);

    virtual void cleanup();
};

#endif // SLICEVIEWER_H
