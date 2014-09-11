#ifndef VERTEXVT_H
#define VERTEXVT_H

#include "Info/ModelInfo.h"

namespace ModelInfo {
    class VertexVT {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat tx;
        GLfloat ty;
        GLfloat tz;

        VertexVT() { }
        VertexVT(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat tx,
                const GLfloat ty,
                const GLfloat tz
                ) :
            x(x), y(y), z(z),
            tx(tx), ty(ty), tz(tz) {
        }
    };

    using VerticesVT = QVector<VertexVT>;
    using VerticesVTPtr = VerticesVT *;

    using VerticesVTPointer = QSharedPointer<VerticesVT>;

    class BuffersVT : public BuffersV {
    public:
        VerticesVTPointer vertices;

        BuffersVT() { }
        BuffersVT(const VerticesVTPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

Q_DECLARE_METATYPE(ModelInfo::BuffersVT)

#endif // VERTEXVT_H
