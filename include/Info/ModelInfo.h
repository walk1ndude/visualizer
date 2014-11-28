#ifndef MODELINFO_H
#define MODELINFO_H

#include "Info/Info.h"
#include "Info/ShaderInfo.h"

namespace ModelInfo {
    using Type = QString;
    using Params = QVariantMap;

    using Model = QPair<Type, Params>;
    using Models = QList<Model>;

    class VertexV {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;

        VertexV() { }
        VertexV(GLfloat x,
                GLfloat y,
                GLfloat z) :
            x(x), y(y), z(z) {
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

        explicit BuffersV() { }
        explicit BuffersV(const VerticesVPointer & vertices,
                          const IndicesPointer & indices) :
            indices(indices),
            vertices(vertices) {
        }
    };
}


#endif // MODELINFO_H
