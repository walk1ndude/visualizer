#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H

#include <QtCore/QMutexLocker>

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "Info/ModelInfo.h"
#include "Info/MaterialInfo.h"
#include "Info/LightInfo.h"
#include "Info/ShaderInfo.h"
#include "Info/TextureInfo.h"
#include "Info/PointsInfo.h"

#include "ViewPort/ViewPort.h"

namespace Model {
    class AbstractModel : public QObject {
        Q_OBJECT
    public:
        virtual ~AbstractModel();

        virtual bool bindShaderProgram() final;
        virtual void releaseShaderProgram() final;

        virtual void rotate(const QVector3D & rotation) final;

        virtual void drawModel(ViewPort::ViewPort & viewPort) final;

        virtual void addMaterial(MaterialInfo::Material * material, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;
        virtual void addLightSource(LightInfo::LightSource * lightSource, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;
        virtual void addTexture(QOpenGLTexture * texture, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;

        template <class Key, class Value>
        void addToMap(QMap<Key, Value *> & map, Key key, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
            QMutexLocker locker(&_modelMutex);

            if (_program) {
                map.insert(key, new Value(_program, shaderVariables));
            }
            else {
                emit shaderProgramSetVariableErrorHappened();
            }
        }

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
            _stride = sizeof(buffers.vertices->at(0));

            _vboVert.bind();
            _vboVert.allocate(buffers.vertices.data()->data(), _vertexCount * _stride);

            if (buffers.indices.data()) {
                _vboInd.setUsagePattern(usagePattern);

                _indexCount = buffers.indices.data()->size();

                _vboInd.bind();
                _vboInd.allocate(buffers.indices.data()->data(), _indexCount * sizeof(GLuint));

                buffers.indices.clear();
            }

            bindShaderVariablesToBuffers(_program);

            _vao.release();
            buffers.vertices.clear();

            releaseShaderProgram();
        }

        virtual QMatrix4x4 model() final;
        virtual void setParent(AbstractModel * parent) final;

    protected:
        QMutex _modelMutex;

        explicit AbstractModel(AbstractModel * parent = nullptr,
                               const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles());
        
        virtual void initShaderVariables(QOpenGLShaderProgram * program) = 0;
        virtual void setShaderVariables(QOpenGLShaderProgram * program, ViewPort::ViewPort & viewPort) = 0;
        virtual void bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) = 0;

        virtual void glStatesEnable() { }
        virtual void glStatesDisable() { }

        virtual bool checkDepthBuffer(ViewPort::ViewPort & viewPort);

        virtual void setChildrenVariables() { }

        virtual int stride() final;

        virtual GLsizei indexCount() final;
        virtual GLsizei vertexCount() final;

        virtual AbstractModel * parent() final;
        virtual void addChild(AbstractModel * child) final;

        virtual void drawModelWithIndices();
        virtual void drawModelWithoutIndices();

        virtual void setShaderVariables();

        virtual QOpenGLShaderProgram * program() final;


    private:
        QOpenGLBuffer _vboVert;
        QOpenGLBuffer _vboInd;

        QOpenGLVertexArrayObject _vao;

        QOpenGLShaderProgram * _program;

        QMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> _materials;
        QMap<LightInfo::LightSource *, LightInfo::LightProgram *> _lightSources;

        QMap<QOpenGLTexture *, TextureInfo::TextureProgram *> _textures;

        uint _modelID;

        ShaderInfo::ShaderFiles _shaderFiles;

        int _stride;

        GLsizei _indexCount;
        GLsizei _vertexCount;

        AbstractModel * _parent;
        QList<AbstractModel *> _children;

        QMatrix4x4 _mMatrix;

        QVector3D _orientation;

        bool initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles);

        void bindTextures();
        void releaseTextures();

    signals:
        void shaderProgramInitErrorHappened();
        void shaderProgramSetVariableErrorHappened();
        void redraw();

    public slots:
        virtual void deleteModel() final;
    };
}

#endif // ABSTRACTMODEL_H
