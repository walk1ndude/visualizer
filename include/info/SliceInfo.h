#ifndef SLICEINFO_H
#define SLICEINFO_H

#include "Info/Info.h"

namespace SliceInfo {
    enum SliceDataType {
        Int8,
        Int16
    };

    using MergedData = uchar;
    using MergedDataPtr = MergedData *;
    using MergedDataPointer = QSharedPointer<MergedData>;

    using HuRange = QVector<int>;
    using Scaling = QVector<float>;
    using Size = QVector<size_t>;

    using Alignment = int;
    using RowLenght = int;

    struct SliceSettings {
        MergedDataPointer mergedData;

        HuRange huRange;
        Scaling scaling;
        Size size;

        Alignment alignment;
        RowLenght rowLength;

        SliceDataType sliceDataType;
    };
}

#endif // SLICEINFO_H
