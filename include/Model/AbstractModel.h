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

#include "Camera/Camera.h"

#include "Model/ModelFactory.h"

namespace Model {
    class AbstractModel : public QObject {
        Q_OBJECT
    public:
        virtual ~AbstractModel();

        virtual int id() const final;

        virtual Camera::ModelMatrix model(const Viewport::Viewport * viewport) const;
        virtual Camera::ViewMatrix view(const Viewport::Viewport * viewport) const;
        virtual Camera::ProjectionMatrix projection(const Viewport::Viewport * viewport) const;
        virtual Camera::ViewMatrix lightView(const Viewport::Viewport * viewport) const;

        virtual Camera::Matrix childsMVP(const Viewport::Viewport * viewport, const AbstractModel * child) const;
        virtual Camera::Matrix mvp(const Viewport::Viewport * viewport) const;
        
        virtual Camera::ScaleMatrix scaleMatrix() const;

        virtual QVector3D orientationEuler() const;
        virtual Camera::Orientation orientationQuat() const;

        virtual QVector3D scale() const;
        virtual QVector3D position() const;

        virtual Camera::NormalMatrix normalMatrix(const Viewport::Viewport * viewport) const;

        virtual bool updateNeeded() const final;

        virtual Scene::AbstractScene * scene() const;

        virtual bool hasDepth() const;

        virtual void lockToWorldAxis() final;
        virtual void lockToModelAxis() final;

        static void registerType(const ModelName & name, ModelFactory * factory);
        static AbstractModel * createModel(const ModelName & name, const ModelParams & params = ModelParams());

        virtual AbstractModel * invokeMethod(const QString & name, const ModelParams & params = ModelParams());

        virtual void init(const ModelParams & params = ModelParams());

    protected:
        QMutex modelMutex;

        QMap<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> attributeArrays;
        QMap<ShaderInfo::ShaderVariableName, ShaderInfo::ShaderVariable> uniformValues;

        explicit AbstractModel(Scene::AbstractScene * scene,
                               AbstractModel * parent = nullptr,
                               const ShaderInfo::ShaderFiles & shaderFiles = ShaderInfo::ShaderFiles(),
                               const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays = ShaderInfo::ShaderVariablesNames(),
                               const ShaderInfo::ShaderVariablesNames & shaderUniformValues = ShaderInfo::ShaderVariablesNames());
        
        virtual void bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewPort) const = 0;
        virtual void bindAttributeArrays(QOpenGLShaderProgram * program) const = 0;

        virtual void bindUniformValues() const;

        virtual void glStatesEnable() const;
        virtual void glStatesDisable() const;

        virtual int stride() const;

        virtual GLsizei indexCount() const;
        virtual GLsizei vertexCount() const;

        virtual AbstractModel * parent() const;
        virtual void addChild(AbstractModel * child) final;

        virtual void drawingRoutine() const;
        virtual void updateRoutine();

        virtual QOpenGLShaderProgram * program() const final;

        virtual void deleteModel();
        
        virtual Camera::Orientation changedOrientation(const Camera::Rotation & rot) const;
        
        virtual Camera::Orientation lockToWorldAxis(const Camera::Rotation & rot) const;
        virtual Camera::Orientation lockToModelAxis(const Camera::Rotation & rot) const;

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

        int _id;

        int _stride;

        bool _depthTest;

        GLsizei _indexCount;
        GLsizei _vertexCount;

        AbstractModel * _parent;
        QList<AbstractModel *> _children;

        Camera::Orientation _orientation;
        
        QVector3D _position;
        QVector3D _scale;

        Camera::ScaleMatrix _scaleM;

        static QHash<ModelName, ModelFactory *> _factories;

        bool _updateNeeded;
        bool _lockToWorldAxis;

        bool initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles);
        void initShaderVariables();

        void bindTextures() const;
        void releaseTextures() const;

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
    };
}

Q_DECLARE_METATYPE(Model::AbstractModel *)

#endif // ABSTRACTMODEL_H
