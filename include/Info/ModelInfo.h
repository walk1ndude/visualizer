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

    using IndicesPtr = QVector<GLuint> *;

    using VerticesVPtr = QVector<VertexV> *;
    using VerticesVNPtr = QVector<VertexVN> *;
    using VerticesVCPtr = QVector<VertexVC> *;
    using VerticesVNCPtr = QVector<VertexVNC> *;
    using VerticesVTPtr = QVector<VertexVT> *;
    using VerticesVNTPtr = QVector<VertexVNT> *;

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

    class BuffersVNC : public BuffersV {
    public:
        VerticesVNCPointer vertices;

        BuffersVNC() { }
        BuffersVNC(const VerticesVNCPointer & vertices,
                   const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

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

    class BuffersVNT : public BuffersV {
    public:
        VerticesVNTPointer vertices;

        BuffersVNT() { }
        BuffersVNT(const VerticesVNTPointer & vertices,
                   const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };

    using ViewAxisRange = QVector2D;

    enum ViewAxis {
        XAXIS,
        YAXIS,
        ZAXIS
    };

    class ViewRange {
    private:
        ShaderInfo::ShaderVariable shaderXRange;
        ShaderInfo::ShaderVariable shaderYRange;
        ShaderInfo::ShaderVariable shaderZRange;
    public:
        ViewAxisRange xRange;
        ViewAxisRange yRange;
        ViewAxisRange zRange;

        ViewRange(const ViewAxisRange & xRange,
                  const ViewAxisRange & yRange,
                  const ViewAxisRange & zRange,
                  QOpenGLShaderProgram * program,
                  const ShaderInfo::ShaderVariablesNames & shaderVariables) {
            this->xRange = xRange;
            this->yRange = yRange;
            this->zRange = zRange;

            shaderXRange = program->uniformLocation(shaderVariables.at(0));
            shaderYRange = program->uniformLocation(shaderVariables.at(1));
            shaderZRange = program->uniformLocation(shaderVariables.at(2));
        }

        void setUniformValue(QOpenGLShaderProgram * program) {
            program->setUniformValue(shaderXRange, xRange);
            program->setUniformValue(shaderYRange, yRange);
            program->setUniformValue(shaderZRange, zRange);
        }

        void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                               const ModelInfo::ViewAxis viewAxis) {
            switch (viewAxis) {
            case ModelInfo::XAXIS:
                xRange = viewAxisRange;
                break;
            case ModelInfo::YAXIS:
                yRange = viewAxisRange;
                break;
            case ModelInfo::ZAXIS:
                zRange = viewAxisRange;
                break;
            }
        }
    };
}


#endif // MODELINFO_H
