#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <QtGui/QMatrix4x4>

typedef struct _ViewPortParams {
    float left;
    float right;
    float bottom;
    float top;
    float nearVal;
    float farVal;
}ViewPortParams;

class MatrixStack {

public:
    void initialize(const ViewPortParams & viewPortParams,
                    const QVector3D & cameraPos = QVector3D(0.0, 0.0, -4.0),
                    const QVector3D & rotationAngles = QVector3D(0.0, 0.0, 0.0),
                    const QVector3D & viewPoint = QVector3D(0.0, 0.0, 0.0),
                    const QVector3D & lookUpVector = QVector3D(0.0, 1.0, 0.0));

    QMatrix4x4 mvpMatrix();

    void setAngles(const QVector3D & rotationAngles);
    void setCameraPos(const QVector3D & cameraPos);
    void setViewPoint(const QVector3D & viewPoint);
    void setLookUpVector(const QVector3D & lookUpVector);

private:
    ViewPortParams _viewPortParams;

    QMatrix4x4 _mMatrix;
    QMatrix4x4 _vMatrix;
    QMatrix4x4 _pMatrix;
    QMatrix4x4 _cameraTransformation;

    QVector3D _rotationAngles;
    QVector3D _orientation;

    QVector3D _cameraPosition;
    QVector3D _cameraViewPoint;
    QVector3D _cameraUpDirection;
    QVector3D _cameralookUpVector;

    QVector3D _cameraPos;
    QVector3D _viewPoint;
    QVector3D _lookUpVector;

    void identity();

    void recalculateRotationMatrix();
    void recalculateCameraVectors();

    void recalculateStack();
};

#endif // MATRIXSTACK_H
