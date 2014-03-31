#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <QtGui/QMatrix4x4>

class MatrixStack {

public:
    QMatrix4x4 modelView();
    QMatrix4x4 projection();

    void identity(const QVector3D & position = QVector3D(0.0, 0.0, 0.0),
                  const QVector3D & orientation = QVector3D(0.0, 0.0, 0.0));

    void ortho(const float & left, const float & right, const float & bottom,
               const float & top, const float & nearVal, const float & farVal);

    void lookAt(const QVector3D & pos, const QVector3D & viewPoint, const QVector3D up);

    void translate(const QVector3D & vec);
    void rotate(const QVector3D & angle);
    void scale(const QVector3D & scale);

private:
    QMatrix4x4 _mMatrix;
    QMatrix4x4 _vMatrix;
    QMatrix4x4 _pMatrix;

    QVector3D _orientation;
    QVector3D _position;

    float anglePi(const float & angle);
};

#endif // MATRIXSTACK_H
