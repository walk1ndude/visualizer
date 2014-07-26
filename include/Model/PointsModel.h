#ifndef POINTSMODEL_H
#define POINTSMODEL_H

#include "Info/PointsInfo.h"

#include "Model/AbstractModel.h"

// extend vertex types, add type which stores x,y,z; r,g,b and polygon id
namespace ModelInfo {
    class VertexVCP {
    public:
        GLfloat x;
        GLfloat y;
        GLfloat z;
        GLfloat r;
        GLfloat g;
        GLfloat b;
        GLfloat p;

        VertexVCP() { }
        VertexVCP(
                const GLfloat x,
                const GLfloat y,
                const GLfloat z,
                const GLfloat r,
                const GLfloat g,
                const GLfloat b,
                const GLfloat p
                ) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->r = r;
            this->g = g;
            this->b = b;
            this->p = p;
        }
    };

    using VerticesVCP = QVector<VertexVCP>;
    using VerticesVCPPtr = VerticesVCP *;

    using VerticesVCPPointer = QSharedPointer<VerticesVCP>;

    class BuffersVCP : public BuffersV {
    public:
        VerticesVCPPointer vertices;

        BuffersVCP() { }
        BuffersVCP(const VerticesVCPPointer & vertices,
                   const IndicesPointer & indices) {
            this->vertices = vertices;
            this->indices = indices;
        }
    };
}

namespace Model {
    class PointsModel : public AbstractModel {
        Q_OBJECT
    public:
        explicit PointsModel(const ShaderInfo::ShaderFiles & shaderFiles =
                ShaderInfo::ShaderFiles(
                    ShaderInfo::VertexShaderFiles() << ShaderInfo::VertexShaderFile(":shaders/pointsVertex.glsl"),
                    ShaderInfo::FragmentShaderFiles() << ShaderInfo::FragmentShaderFile(":shaders/pointsFragment.glsl"),
                    ShaderInfo::GeometryShaderFiles() << ShaderInfo::GeometryShaderFile(":shaders/pointsGeometry.glsl")
                ));

        void init(const PointsInfo::FacePoints & facePoints);
        void update(const PointsInfo::FacePoints & facePoints);
    };
}

#endif // POINTSMODEL_H
