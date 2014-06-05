#ifndef SLICEINFO_H
#define SLICEINFO_H

#include <QtCore/QSharedPointer>

#include <QtCore/QVector>

namespace SliceInfo {
    enum SliceDataType {
        Int8,
        Int16
    };

    struct SliceSettings {
        QSharedPointer<uchar> mergedData;

        QVector<int> huRange;

        QVector<float> scaling;

        QVector<size_t> size;

        int alignment;

        int rowLength;

        SliceDataType sliceDataType;
    };
}

#endif // SLICEINFO_H
