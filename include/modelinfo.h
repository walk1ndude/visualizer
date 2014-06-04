#ifndef MODELINFO_H
#define MODELINFO_H

#include <QtCore/QSharedPointer>
#include <QtCore/QVector>

#include <QtGui/qopengl.h>

namespace ModelInfo {
    typedef struct _ModelVertex {
        GLfloat x;
        GLfloat y;
        GLfloat z;

        GLfloat nx;
        GLfloat ny;
        GLfloat nz;
    } ModelVertex;

    typedef struct _ModelBuffers {
        QSharedPointer<QVector<ModelVertex> > vertices;
    } ModelBuffers;
}


#endif // MODELINFO_H
