#ifndef SLICEINFO_H
#define SLICEINFO_H

#include <QtCore/QSharedPointer>

namespace SliceInfo {
    typedef enum _SliceDataType {
        Int8,
        Int16
    } SliceDataType;

    typedef struct _SliceSettings {
        QSharedPointer<uchar> mergedData;

        QVector<int> huRange;

        QVector<float> scaling;

        QVector<size_t> size;

        int alignment;

        size_t rowLength;

        SliceDataType sliceDataType;
    } SliceSettings;
}

#endif // SLICEINFO_H
