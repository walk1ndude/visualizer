#ifndef HELPERS_H
#define HELPERS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QtCore/QDebug>

#include <QtQuick/QQuickItem>

#define RADON_DEGREE_RANGE 180
#define PI_TIMES_2 (2 * CV_PI)
#define toRad(x) ((x) * CV_PI / 180.0)

#define FROM_16_TO_8 0.00390625
#define FROM_8_TO_16 256.0

template<class Input, class OutputVector>
OutputVector scaleVector(const Input & sX, const Input & sY, const Input & sZ) {
    Input sSX = sX * sX;
    Input sSY = sY * sY;
    Input sSZ = sZ * sZ;

    Input _sX = sqrt(sSX + sSY);
    Input _sY = sqrt(sSY + sSZ);
    Input _sZ = sqrt(sSX + sSZ);

    Input divisor;

    if (_sX > _sY) {
        divisor = (_sX > _sZ) ? _sX : _sZ;
    }
    else {
        divisor = (_sY > _sZ) ? _sY : _sZ;
    }

    return OutputVector(
                sX / divisor,
                sY / divisor,
                sZ / divisor
                );
}

template<class T>
T normalize(T & x, const T & minV, const T & maxV, const T & newMinV = (T) -1.0, const T & newMaxV = (T) 1.0) {
    return (x - minV) * (newMaxV - newMinV) / (maxV - minV) + newMinV;
}



#endif // HELPERS_H
