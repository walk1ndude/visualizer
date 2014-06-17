#ifndef HELPERS_H
#define HELPERS_H

template<class Input, class OutputVector>
OutputVector scaleVector(const Input & sX, const Input & sY, const Input & sZ) {
    QVector3D scale;
    Input divisor;

    if (sX > sY) {
        divisor = (sX > sZ) ? sX : sZ;
    }
    else {
        divisor = (sY > sZ) ? sY : sZ;
    }

    scale.setX(sX / divisor);
    scale.setY(sY / divisor);
    scale.setZ(sZ / divisor);

    return scale;
}

template<class T>
T normalize(T & x, const T & minV, const T & maxV, const T & newMinV = (T) -1.0, const T & newMaxV = (T) 1.0) {
    return (x - minV) * (newMaxV - newMinV) / (maxV - minV) + newMinV;
}



#endif // HELPERS_H
