#include "Model/HeadModel.h"

namespace Model {
    HeadModel::HeadModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(shaderFiles) {

    }

    void HeadModel::init(const int & depth) {
        _vertexCount = 4 * depth;
        _indexCount = 6 * depth;

        ModelInfo::VerticesVT vertices;
        ModelInfo::Indices indices;

        GLfloat step = 2.0 / (GLfloat) depth;
        GLfloat stepTexture = 1.0 / (GLfloat) depth;

        GLfloat zCurrent = -1.0;
        GLfloat zCurrentTexture = 0.0;

        for (int i = 0; i != depth; ++ i) {
            vertices.push_back(ModelInfo::VertexVT(-1.0, -1.0, zCurrent, 0.0, 1.0, zCurrentTexture));
            vertices.push_back(ModelInfo::VertexVT(-1.0, 1.0, zCurrent, 0.0, 0.0, zCurrentTexture));
            vertices.push_back(ModelInfo::VertexVT(1.0, 1.0, zCurrent, 1.0, 0.0, zCurrentTexture));
            vertices.push_back(ModelInfo::VertexVT(1.0, -1.0, zCurrent, 1.0, 1.0, zCurrentTexture));

            indices.push_back(4 * i);
            indices.push_back(4 * i + 2);
            indices.push_back(4 * i + 1);
            indices.push_back(4 * i);
            indices.push_back(4 * i + 3);
            indices.push_back(4 * i + 2);

            zCurrent += step;
            zCurrentTexture += stepTexture;
        };

        ModelInfo::BuffersVT buffers;

        buffers.vertices = ModelInfo::VerticesVTPointer(&vertices);
        buffers.indices = ModelInfo::IndicesPointer(&indices);

        createModel<ModelInfo::BuffersVT>(buffers);
    }

    void HeadModel::initShaderVariables() {
        _shaderView = _program->uniformLocation("view");
        _shaderModel = _program->uniformLocation("model");
        _shaderProjection = _program->uniformLocation("projection");
        _shaderScale = _program->uniformLocation("scale");
        _shaderStep = _program->uniformLocation("step");
    }

    void HeadModel::setShaderVariables(Render::ViewPort & viewPort) {

    }

    void HeadModel::initModel(ModelInfo::BuffersV & buffers) {
        _shaderVertex = _program->attributeLocation("vertex");
        _shaderTexHead = _program->attributeLocation("texHead");

        _program->enableAttributeArray(_shaderVertex);
        _program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, sizeof(buffers.vertices->at(0)));

        _program->enableAttributeArray(_shaderTexHead);
        _program->setAttributeBuffer(_shaderTexHead, GL_FLOAT, sizeof(GLfloat) * 3, 3, sizeof(buffers.vertices->at(0)));
    }
}
