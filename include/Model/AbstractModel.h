#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "Info/ModelInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/LightInfo.h"
#include "Info/ShaderInfo.h"
#include "Info/TextureInfo.h"

#include "ViewPort/ViewPort.h"

namespace Model {
    class AbstractModel : public QObject {
        Q_OBJECT
    public:
        explicit AbstractModel(const ShaderInfo::ShaderFiles & shaderFiles);
        virtual ~AbstractModel();

        virtual void initShaderVariables() = 0;
        virtual void setShaderVariables(ViewPort::ViewPort & viewPort) = 0;
        virtual void bindShaderVariablesToBuffers() = 0;

        virtual void glStatesEnable() = 0;
        virtual void glStatesDisable() = 0;

        virtual bool bindShaderProgram() final;
        virtual void releaseShaderProgram() final;

        virtual void drawModel(ViewPort::ViewPort & viewPort) final;

        virtual void addMaterial(MaterialInfo::Material * material, const ShaderInfo::ShaderVariables & shaderVariables) final;
        virtual void addLightSource(LightInfo::LightSource * lightSource, const ShaderInfo::ShaderVariables & shaderVariables) final;
        virtual void addTexture(QOpenGLTexture * texture, const ShaderInfo::ShaderVariables & shaderVariables) final;

        template <class Key, class Value>
        void addToMap(QMap<Key, Value *> & map, Key key, const ShaderInfo::ShaderVariables & shaderVariables) {
            QMutexLocker locker(&_modelMutex);

            if (_program) {
                map.insert(key, new Value(_program, shaderVariables));
            }
            else {
                emit shaderProgramSetVariableErrorHappened();
            }
        }

        virtual void setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                  const ModelInfo::ViewAxisRange & yRange,
                                  const ModelInfo::ViewAxisRange & zRange,
                                  const ShaderInfo::ShaderVariables & shaderVariables) final;

        virtual void setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                      const ModelInfo::ViewAxis viewAxis = ModelInfo::XAXIS) final;

        virtual uint modelID() final;

        template <class BuffersT>
        void initModel(BuffersT buffers, const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::StaticDraw) {
            if (!initShaderProgram(_shaderFiles)) {
                emit shaderProgramInitErrorHappened();
                return;
            }

            _vao.create();
            _vboVert.create();
            _vboInd.create();

            updateModel(buffers, usagePattern);
        }

        template <class BuffersT>
        void updateModel(BuffersT buffers, const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::DynamicDraw) {
            bindShaderProgram();

            _vao.bind();

            _vboVert.setUsagePattern(usagePattern);

            _vertexCount = buffers.vertices.data()->size();

            _vboVert.bind();
            _vboVert.allocate(buffers.vertices.data()->data(), _vertexCount * sizeof(buffers.vertices->at(0)));

            if (buffers.indices.data()) {
                _vboInd.setUsagePattern(usagePattern);

                _indexCount = buffers.indices.data()->size();

                _vboInd.bind();
                _vboInd.allocate(buffers.indices.data()->data(), _indexCount * sizeof(GLuint));

                buffers.indices.clear();
            }

            bindShaderVariablesToBuffers();

            _vao.release();
            buffers.vertices.clear();

            releaseShaderProgram();
        }


    protected:
        QOpenGLBuffer _vboVert;
        QOpenGLBuffer _vboInd;

        QOpenGLVertexArrayObject _vao;

        QOpenGLShaderProgram * _program;

        ModelInfo::ViewRange * _viewRange;

        GLsizei _indexCount;
        GLsizei _vertexCount;

        QMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> _materials;
        QMap<LightInfo::LightSource *, LightInfo::LightProgram *> _lightSources;

        QMap<QOpenGLTexture *, TextureInfo::TextureProgram *> _textures;

        QMutex _modelMutex;

        ShaderInfo::ShaderFiles _shaderFiles;

    private:
        uint _modelID;

        bool initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles);
        void setShaderVariables();

        void drawModelWithIndices();
        void drawModelWithoutIndices();

        void bindTextures();
        void releaseTextures();

    signals:
        void shaderProgramInitErrorHappened();
        void shaderProgramSetVariableErrorHappened();

    public slots:
        virtual void deleteModel() final;
    };
}

#endif // ABSTRACTMODEL_H
