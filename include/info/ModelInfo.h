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

    class VertexVN {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat nx;
        GLfloat ny;
        GLfloat nz;

        VertexVN() { }
        VertexVN(GLfloat x,
                 GLfloat y,
                 GLfloat z,
                 GLfloat nx,
                 GLfloat ny,
                 GLfloat nz) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
        }
    };

    class VertexVC {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat cx;
        GLfloat cy;
        GLfloat cz;

        VertexVC() { }
        VertexVC(GLfloat x,
                 GLfloat y,
                 GLfloat z,
                 GLfloat cx,
                 GLfloat cy,
                 GLfloat cz) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->cx = cx;
            this->cy = cy;
            this->cz = cz;
        }
    };

    class VertexVNC {
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

        VertexVNC() { }
        VertexVNC(GLfloat x,
                  GLfloat y,
                  GLfloat z,
                  GLfloat nx,
                  GLfloat ny,
                  GLfloat nz,
                  GLfloat cx,
                  GLfloat cy,
                  GLfloat cz) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            this->cx = cx;
            this->cy = cy;
            this->cz = cz;
        }
    };

    class VertexVT {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat tx;
        GLfloat ty;
        GLfloat tz;

        VertexVT() { }
        VertexVT(GLfloat x,
                 GLfloat y,
                 GLfloat z,
                 GLfloat tx,
                 GLfloat ty,
                 GLfloat tz) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
        }
    };

    class VertexVNT {
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

        VertexVNT() { }
        VertexVNT(GLfloat x,
                 GLfloat y,
                 GLfloat z,
                 GLfloat nx,
                 GLfloat ny,
                 GLfloat nz,
                 GLfloat tx,
                 GLfloat ty,
                 GLfloat tz) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
        }
    };

    using Indices = QVector<GLuint>;

    using VerticesV = QVector<VertexV>;
    using VerticesVN = QVector<VertexVN>;
    using VerticesVC = QVector<VertexVC>;
    using VerticesVNC = QVector<VertexVNC>;
    using VerticesVT = QVector<VertexVT>;
    using VerticesVNT = QVector<VertexVNT>;

    using IndicesPointer = QSharedPointer<Indices>;

    using VerticesVPointer = QSharedPointer<VerticesV>;
    using VerticesVNPointer = QSharedPointer<VerticesVN>;
    using VerticesVCPointer = QSharedPointer<VerticesVC>;
    using VerticesVNCPointer = QSharedPointer<VerticesVNC>;
    using VerticesVTPointer = QSharedPointer<VerticesVT>;
    using VerticesVNTPointer = QSharedPointer<VerticesVNT>;

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

    class BuffersVN : BuffersV {
    public:
        VerticesVNPointer vertices;

        BuffersVN() { }
        BuffersVN(const VerticesVNPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    class BuffersVC : BuffersV {
    public:
        VerticesVCPointer vertices;

        BuffersVC() { }
        BuffersVC(const VerticesVCPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    class BuffersVNC : BuffersV {
    public:
        VerticesVNCPointer vertices;

        BuffersVNC() { }
        BuffersVNC(const VerticesVNCPointer & vertices,
                   const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    class BuffersVT : BuffersV {
    public:
        VerticesVTPointer vertices;

        BuffersVT() { }
        BuffersVT(const VerticesVTPointer & vertices,
                  const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    class BuffersVNT : BuffersV {
    public:
        VerticesVNTPointer vertices;

        BuffersVNT() { }
        BuffersVNT(const VerticesVNTPointer & vertices,
                   const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    using TexelAxisRange = QVector2D;

    enum TexelAxis {
        SAXIS,
        TAXIS,
        PAXIS
    };

    class TexelRange {
    private:
        ShaderInfo::ShaderVariable shaderSRange;
        ShaderInfo::ShaderVariable shaderTRange;
        ShaderInfo::ShaderVariable shaderPRange;
    public:
        TexelAxisRange sRange;
        TexelAxisRange tRange;
        TexelAxisRange pRange;

        TexelRange(const TexelAxisRange & sRange,
                   const TexelAxisRange & tRange,
                   const TexelAxisRange & pRange,
                   QOpenGLShaderProgram * program,
                   const QStringList & shaderVariables) {
            this->sRange = sRange;
            this->tRange = tRange;
            this->pRange = pRange;

            shaderSRange = program->uniformLocation(shaderVariables.at(0));
            shaderTRange = program->uniformLocation(shaderVariables.at(1));
            shaderPRange = program->uniformLocation(shaderVariables.at(2));
        }

        void setUniformValue(QOpenGLShaderProgram * program) {
            program->setUniformValue(shaderSRange, sRange);
            program->setUniformValue(shaderTRange, tRange);
            program->setUniformValue(shaderPRange, pRange);
        }

        void setTexelAxisRange(const ModelInfo::TexelAxisRange & texelAxisRange,
                               const ModelInfo::TexelAxis texelAxis) {
            switch (texelAxis) {
            case ModelInfo::SAXIS:
                sRange = texelAxisRange;
                break;
            case ModelInfo::TAXIS:
                pRange = texelAxisRange;
                break;
            case ModelInfo::PAXIS:
                tRange = texelAxisRange;
                break;
            }
        }
    };
}


#endif // MODELINFO_H
