#ifndef MODELINFO_H
#define MODELINFO_H

#include <QtCore/QSharedPointer>

#include <QtGui/QVector4D>
#include <QtGui/qopengl.h>

namespace ModelInfo {
    class ModelVertexV {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };

    class ModelVertexVN {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;
    };

    class ModelVertexVC {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat cx;
        GLfloat cy;
        GLfloat cz;
    };

    class ModelVertexVNC {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;
        GLfloat cx;
        GLfloat cy;
        GLfloat cz;
    };

    class ModelVertexVT {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat tx;
        GLfloat ty;
        GLfloat tz;
    };

    class ModelVertexVNT {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;
        GLfloat tx;
        GLfloat ty;
        GLfloat tz;
    };

    class Material {
    public:
        QVector4D emmisive;
        QVector4D diffuse;
        QVector4D specular;

        float shininess;
    };

    class ModelBuffers {
    public:
        QSharedPointer<QVector<GLuint> > indices;
        QSharedPointer<QVector<ModelVertexV> > vertices;
    };

    class ModelBuffersV : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexV> > vertices;
    };

    class ModelBuffersVN : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexVN> > vertices;
    };

    class ModelBuffersVNC : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexVNC> > vertices;
    };

    class ModelBuffersVC : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexVC> > vertices;
    };

    class ModelBuffersVT : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexVT> > vertices;
    };

    class ModelBuffersVNT : ModelBuffers {
    public:
        QSharedPointer<QVector<ModelVertexVNT> > vertices;
    };
}


#endif // MODELINFO_H
