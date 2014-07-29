#ifndef MODELINFO_H
#define MODELINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace ModelInfo {
    class VertexV {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;

        VertexV() { }
        VertexV(GLfloat x,
                GLfloat y,
                GLfloat z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }
    };

    using Indices = QVector<GLuint>;
    using IndicesPtr = Indices *;
    using IndicesPointer = QSharedPointer<Indices>;

    using VerticesV = QVector<VertexV>;
    using VerticesVPtr = VerticesV *;
    using VerticesVPointer = QSharedPointer<VerticesV>;

    class BuffersV {
    public:
        IndicesPointer indices;
        VerticesVPointer vertices;

        BuffersV() { }
        BuffersV(const VerticesVPointer & vertices,
                 const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}


#endif // MODELINFO_H
