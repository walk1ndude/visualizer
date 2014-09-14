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

#include "Scene/AbstractScene.h"

namespace Model {
    class AbstractModel : public QObject {
        Q_OBJECT
    public:
        virtual ~AbstractModel();

        virtual int id() final;

        virtual QMatrix4x4 model(const Viewport::Viewport * viewport = nullptr);
        virtual QMatrix4x4 view(const Viewport::Viewport * viewport);
        virtual QMatrix4x4 projection(const Viewport::Viewport * viewport);
        virtual QMatrix4x4 scaleMatrix();

        virtual QVector3D orientationEuler();
        virtual QQuaternion orientationQuat();

        virtual QVector3D scale();
        virtual QVector3D position();

        virtual QVector3D imageSpacings() const;

        virtual QMatrix3x3 normalMatrix(const Viewport::Viewport * viewport);

        virtual bool depthTest();

        virtual bool updateNeeded() final;

        virtual Scene::AbstractScene * scene() final;

    protected:
        QMutex modelMutex;

        QMap<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> attributeArrays;
        QMap<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> uniformValues;

        explicit AbstractModel(Scene::AbstractScene * scene,
                               AbstractModel * parent = nullptr,
                               const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(),
                               const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays = ShaderInfo::ShaderVariablesNames(),
                               const ShaderInfo::ShaderVariablesNames & shaderUniformValues = ShaderInfo::ShaderVariablesNames());
        
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewPort) = 0;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) = 0;

        virtual void glStatesEnable();
        virtual void glStatesDisable();

        virtual int stride() final;

        virtual GLsizei indexCount() final;
        virtual GLsizei vertexCount() final;

        virtual AbstractModel * parent() final;
        virtual void addChild(AbstractModel * child) final;

        virtual void drawingRoutine();
        virtual void updateRoutine();

        virtual void bindUniformValues();

        virtual QOpenGLShaderProgram * program() final;

        virtual void deleteModel();

        template <class BuffersT>
        void fillBuffers(BuffersT buffers, const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::StaticDraw) {
            if (!_program) {
                if (!initShaderProgram(_shaderFiles)) {
                    emit shaderProgramInitErrorHappened();
                    return;
                }

                _vao.create();
                _vboVert.create();
                _vboInd.create();
            }

            bindShaderProgram();

            _vao.bind();

            _vboVert.setUsagePattern(usagePattern);

            _vertexCount = buffers.vertices.data()->size();

            if (_vertexCount) {
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

                bindAttributeArrays(_program);
            } else {
                buffers.indices.clear();
            }

            _vao.release();
            buffers.vertices.clear();

            releaseShaderProgram();
        }

    private:
        QOpenGLBuffer _vboVert;
        QOpenGLBuffer _vboInd;

        QOpenGLVertexArrayObject _vao;

        QOpenGLShaderProgram * _program;

        Scene::AbstractScene * _scene;

        QMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> _materials;
        QMap<LightInfo::LightSource *, LightInfo::LightProgram *> _lightSources;

        QMap<QOpenGLTexture *, TextureInfo::TextureProgram *> _textures;

        ShaderInfo::ShaderFiles _shaderFiles;

        ModelInfo::ImageSpacings _imageSpacings;

        int _id;

        int _stride;

        bool _depthTest;

        GLsizei _indexCount;
        GLsizei _vertexCount;

        AbstractModel * _parent;
        QList<AbstractModel *> _children;

        QQuaternion _orientation;
        QVector3D _position;
        QVector3D _scale;

        QMatrix4x4 _scaleM;

        bool _updateNeeded;

        bool initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles);

        virtual void initShaderVariables() final;

        void bindTextures();
        void releaseTextures();

        template <class Key, class Value>
        void addToMap(QMap<Key, Value *> & map, Key key, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
            QMutexLocker locker(&modelMutex);

            if (_program) {
                map.insert(key, new Value(_program, shaderVariables));
            }
            else {
                emit shaderProgramSetVariableErrorHappened();
            }
        }

    signals:
        void shaderProgramInitErrorHappened();
        void shaderProgramSetVariableErrorHappened();

        void childUpdated();

    public slots:
        virtual bool bindShaderProgram() final;
        virtual void releaseShaderProgram() final;

        virtual void rotate(const QVector3D & rotation, const qreal & speed = 0.5f);

        virtual void translate(const QVector3D & translation);

        virtual void scale(const QVector3D & scale);

        virtual void drawModel(const Viewport::Viewport * viewPort) final;

        virtual void addMaterial(MaterialInfo::Material * material, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;
        virtual void addLightSource(LightInfo::LightSource * lightSource, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;
        virtual void addTexture(QOpenGLTexture * texture, const ShaderInfo::ShaderVariablesNames & shaderVariables) final;

        virtual void setParent(AbstractModel * parent) final;

        virtual void queueForUpdate() final;

        virtual bool checkDepthBuffer(const Viewport::Viewport * viewPort);

        virtual void update();

        virtual void addPoint(const PointsInfo::Name & name, PointsInfo::ModelPoint * point);
        virtual void hidePoint(const PointsInfo::Name & point);

        virtual void setImageSpacings(const ModelInfo::ImageSpacings & imageSpacings);
    };
}

#endif // ABSTRACTMODEL_H
