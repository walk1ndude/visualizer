#ifndef VERTEXVC_H
#define VERTEXVC_H

#include "Info/ModelInfo.h"

namespace ModelInfo {
    class VertexVC {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;

        VertexVC() { }
        VertexVC(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat r,
                const GLfloat g,
                const GLfloat b
                ) :
            x(x), y(y), z(z),
            r(r), g(g), b(b) {
        }
    };

    using VerticesVC = QVector<VertexVC>;
    using VerticesVCPtr = VerticesVC *;

    using VerticesVCPointer = QSharedPointer<VerticesVC>;

    class BuffersVC : public BuffersV {
    public:
        VerticesVCPointer vertices;

        BuffersVC() { }
        BuffersVC(const VerticesVCPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

Q_DECLARE_METATYPE(ModelInfo::BuffersVC)

#endif // VERTEXVC_H
