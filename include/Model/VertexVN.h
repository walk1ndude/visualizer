#ifndef VERTEXVN_H
#define VERTEXVN_H

#include "Info/ModelInfo.h"

namespace ModelInfo {
    class VertexVN {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;

        VertexVN() { }
        VertexVN(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat nx,
                const GLfloat ny,
                const GLfloat nz
                )  :
            x(x), y(y), z(z),
            nx(nx), ny(ny), nz(nz) {
        }
    };

    using VerticesVN = QVector<VertexVN>;
    using VerticesVNPtr = VerticesVN *;

    using VerticesVNPointer = QSharedPointer<VerticesVN>;

    class BuffersVN : public BuffersV {
    public:
        VerticesVNPointer vertices;

        BuffersVN() { }
        BuffersVN(const VerticesVNPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

Q_DECLARE_METATYPE(ModelInfo::BuffersVN)

#endif // VERTEXVN_H
