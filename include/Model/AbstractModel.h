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

        virtual uint id() const final;

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

        virtual void addChild(AbstractModel * child) final;

        virtual bool hasDepth() const;

        virtual void lockToWorldAxis() final;
        virtual void lockToModelAxis() final;

        static void registerType(const ModelInfo::Type & name, ModelFactory * factory);
        static AbstractModel * createModel(const ModelInfo::Type & name,
                                           Scene::AbstractScene * scene,
                                           AbstractModel * parent = nullptr);

        virtual void init(const ModelInfo::Params & params = ModelInfo::Params());

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

        virtual void drawingRoutine() const;
        virtual void updateRoutine();

        virtual QOpenGLShaderProgram * program() const final;

        virtual void deleteModel();
        
        virtual Camera::Orientation changedOrientation(const Camera::Rotation & rot) const;
        
        virtual Camera::Orientation lockToWorldAxis(const Camera::Rotation & rot) const;
        virtual Camera::Orientation lockToModelAxis(const Camera::Rotation & rot) const;

        template <class BuffersT>
        void fillBuffers(BuffersT buffers, const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::StaticDraw) {
            bindShaderProgram();

            _vao.bind();

            _vboVert.setUsagePattern(usagePattern);

            _vertexCount = buffers.vertices.data()->size();

            if (_vertexCount) {
                _stride = sizeof(buffers.vertices->at(0));

                _vboVert.bind();
                _vboVert.allocate(buffers.vertices.data()->data(), _vertexCount * _stride);

                if (buffers.indices.data()) {
                    if (!_vboInd.isCreated()) {
                        _vboInd.create();
                    }

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

        static QHash<ModelInfo::Type, ModelFactory *> _factories;

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

        template <class T, class U>
        void initFromParams(const ModelInfo::Params & params, const QStringList & initializationOrder,
                            void (AbstractModel::*addToElems)(T, const ShaderInfo::ShaderVariablesNames &),
                            Scene::AbstractScene * scene,
                            T (Scene::AbstractScene::*findObject)(const U &) const) {
            ShaderInfo::ShaderVariablesNames variables;

            QVariantMap paramList;

            T object;

            for (const QString & objectID : params.keys()) {
                variables.clear();

                paramList = params[objectID].toMap();

                for (const QString & paramInOrder : initializationOrder) {
                    variables << paramList[paramInOrder].value<ShaderInfo::ShaderVariableName>();
                }

                object = (scene->*findObject)(objectID);

                if (object) {
                    (this->*addToElems)(object, variables);
                }
            }
        }

    signals:
        void shaderProgramInitErrorHappened();
        void shaderProgramSetVariableErrorHappened();

        void childUpdated();

        void post(const Message::SettingsMessage & message);

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
        virtual void togglePoint(const PointsInfo::Name & point);

        virtual void invoke(const QString & name, const ModelInfo::Params & params = ModelInfo::Params());
    };
}

Q_DECLARE_METATYPE(Model::AbstractModel *)

#endif // ABSTRACTMODEL_H
