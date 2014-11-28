#ifndef INFO_H
#define INFO_H

#include <QtCore/QDebug>
#include <QtCore/QSharedPointer>

#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QColor>

#include <QtQuick/QQuickItem>

namespace PointsInfo {
    using Position3D = QVector3D;
    using Position4D = QVector4D;
}

template <class T, class C>
T getNewID(C & currentCounter) {
    return T::number(currentCounter ++);
}

#endif // INFO_H
