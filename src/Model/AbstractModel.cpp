#include "Model/AbstractModel.h"

#include <cmath>

static uint modelNumber = 0;

namespace Model {
    QHash<ModelInfo::Type, ModelFactory *> AbstractModel::_factories;

    AbstractModel::AbstractModel(Scene::AbstractScene * scene,
                                 const ShaderInfo::ShaderFiles & shaderFiles,
                                 const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                 const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        Scene::SceneObject(Scene::getNewID(modelNumber)) {

        _numberedID = modelNumber - 1;

        _vboVert = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        _vboInd = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

        _program = nullptr;
        _scene = scene;

        _stride = 0;

        _indexCount = 0;
        _vertexCount = 0;

        _updateNeeded = false;
        _lockToWorldAxis = true;

        for (const ShaderInfo::ShaderVariableName & attrArray : shaderAttributeArrays) {
            attributeArrays.insert(attrArray, -1);
        }

        for (const ShaderInfo::ShaderVariableName & value : shaderUniformValues) {
            uniformValues.insert(value, -1);
        }

        if (!_program) {
            if (!initShaderProgram(shaderFiles)) {
                emit shaderProgramInitErrorHappened();
                return;
            }

            _vao.create();
            _vboVert.create();
        }
    }

    AbstractModel::~AbstractModel() {
        deleteModel();
    }

    void AbstractModel::init(const ModelInfo::Params & params) {
        initFromParams<Scene::LightSource *>(
                    params["lights"].toMap(),
                    Scene::LightSource::initializationOrder,
                    &AbstractModel::addLightSource,
                    &Scene::AbstractScene::lightSource
                );

        initFromParams<Scene::Material *>(
                    params["materials"].toMap(),
                    Scene::Material::initializationOrder,
                    &AbstractModel::addMaterial,
                    &Scene::AbstractScene::material
                    );

        initFromParams<Scene::Texture *>(
                    params["textures"].toMap(),
                    Scene::Texture::initializationOrder,
                    &AbstractModel::addTexture,
                    &Scene::AbstractScene::texture
                    );
    }

    void AbstractModel::registerType(const ModelInfo::Type & name, ModelFactory * factory) {
        _factories[name] = factory;
    }

    AbstractModel * AbstractModel::createModel(const ModelInfo::Type & name, Scene::AbstractScene * scene) {
        return _factories[name]->createModel(scene);
    }

    uint AbstractModel::numberedID() const {
        return _numberedID;
    }

    Scene::AbstractScene * AbstractModel::scene() const {
        return _scene;
    }

    void AbstractModel::deleteModel() {
        QMutexLocker locker(&modelMutex);

        if (_program) {
            delete _program;
        }

        _vao.destroy();

        _vboVert.destroy();
        _vboInd.destroy();

        qDeleteAll(_lightSources);
        qDeleteAll(_materials);
        qDeleteAll(_textures);
    }

    void AbstractModel::addMaterial(Scene::Material * material, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<Scene::Material *, Scene::MaterialProgram>(_materials, material, shaderVariables);
    }

    void AbstractModel::addLightSource(Scene::LightSource * lightSource, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<Scene::LightSource *, Scene::LightProgram>(_lightSources, lightSource, shaderVariables);
    }

    void AbstractModel::addTexture(Scene::Texture * texture, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<Scene::Texture *, Scene::TextureProgram>(_textures, texture, shaderVariables);
    }

    int AbstractModel::stride() const {
        return _stride;
    }

    GLsizei AbstractModel::vertexCount() const {
        return _vertexCount;
    }

    GLsizei AbstractModel::indexCount() const {
        return _indexCount;
    }

    AbstractModel * AbstractModel::parent() const {
        return _parent;
    }
    
    void AbstractModel::setParent(AbstractModel * parent) {
        _parent = parent;
    }

    void AbstractModel::addChild(AbstractModel * child) {
        QMutexLocker locker (&modelMutex);
        child->setParent(this);
        _children.append(child);
    }

    Camera::ModelMatrix AbstractModel::model(const Viewport::Viewport *) const {
        Camera::ModelMatrix mMatrix;
        mMatrix.translate(_position);
        mMatrix.rotate(_orientation);

        return mMatrix;
    }

    Camera::Matrix AbstractModel::mvp(const Viewport::Viewport * viewport) const {
        QMutexLocker locker(&modelMutex);
        return projection(viewport) * view(viewport) * model(viewport);
    }

    QVector3D AbstractModel::orientationEuler() const {
        qreal q1 = _orientation.scalar();
        qreal q2 = _orientation.x();
        qreal q3 = _orientation.y();
        qreal q4 = _orientation.z();

        qreal q13 = q1 * q3;
        qreal q24 = q2 * q4;
        qreal q14 = q1 * q4;
        qreal q23 = q2 * q3;

        return QVector3D(atan2(q13 + q24, q14 - q23),
                         acos(-q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4),
                         atan2(q13 - q24, q23 + q14));
    }

    Camera::Orientation AbstractModel::orientationQuat() const {
        return _orientation;
    }

    QVector3D AbstractModel::scale() const {
        return _scale;
    }

    QVector3D AbstractModel::position() const {
        return _position;
    }

    Camera::ViewMatrix AbstractModel::view(const Viewport::Viewport * viewport) const {
        QMutexLocker locker (&modelMutex);
        return viewport->view();
    }

    Camera::ProjectionMatrix AbstractModel::projection(const Viewport::Viewport * viewport) const {
        QMutexLocker locker (&modelMutex);
        return viewport->projection();
    }
    
    Camera::ViewMatrix AbstractModel::lightView(const Viewport::Viewport * viewport) const {
        QMutexLocker locker (&modelMutex);
        return viewport->view();
    }

    Camera::NormalMatrix AbstractModel::normalMatrix(const Viewport::Viewport * viewport) const {
        QMutexLocker locker (&modelMutex);
        return model(viewport).normalMatrix();
    }

    Camera::ScaleMatrix AbstractModel::scaleMatrix() const {
        QMutexLocker locker (&modelMutex);
        return _scaleM;
    }

    bool AbstractModel::checkDepthBuffer(const Viewport::Viewport * ) {
        QMutexLocker locker (&modelMutex);
        return false;
    }

    void AbstractModel::translate(const QVector3D & translation) {
        QMutexLocker locker (&modelMutex);
        _position += translation;
    }

    void AbstractModel::scale(const QVector3D & scale) {
        QMutexLocker locker (&modelMutex);
        if (_scale.isNull()) {
            _scale = scale;
        }
        else {
            _scale *= scale;
        }

        _scaleM.scale(_scale);
    }

    void AbstractModel::lockToModelAxis() {
        QMutexLocker locker (&modelMutex);
        _lockToWorldAxis = false;
    }

    void AbstractModel::lockToWorldAxis() {
        QMutexLocker locker (&modelMutex);
        _lockToWorldAxis = true;
    }

    void AbstractModel::rotate(const QVector3D & angle, const qreal & speed) {
        QMutexLocker locker (&modelMutex);
        _orientation = changedOrientation(
            Camera::Rotation::fromAxisAndAngle(1.0f, 0.0f, 0.0f, angle.x() * speed) *
            Camera::Rotation::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angle.y() * speed) *
            Camera::Rotation::fromAxisAndAngle(0.0f, 0.0f, 1.0f, angle.z() * speed)
                                          );

        for (AbstractModel * child : childModels()) {
            child->rotate(angle, speed);
        }
    }
    
    Camera::Orientation AbstractModel::changedOrientation(const Camera::Rotation & rot) const{
        return _lockToWorldAxis ? lockToWorldAxis(rot) : lockToModelAxis(rot);
    }
    
    Camera::Orientation AbstractModel::lockToWorldAxis(const Camera::Rotation & rot) const {
        return _orientation * (_orientation.conjugate() / _orientation.lengthSquared() * rot) * _orientation;
    }
    
    Camera::Orientation AbstractModel::lockToModelAxis(const Camera::Rotation & rot) const {
        return _orientation * rot;
    }

    bool AbstractModel::bindShaderProgram() {
        return _program->bind();
    }

    void AbstractModel::releaseShaderProgram() {
        _program->release();
    }

    void AbstractModel::processTextures(void (QOpenGLTexture::*process)(uint, QOpenGLTexture::TextureUnitReset)) const {
        QMapIterator<Scene::Texture *, Scene::TextureProgram *> it(_textures);

        QOpenGLTexture * current;

        while (it.hasNext()) {
            it.next();
            current = it.key()->texture();

            (current->*process)(current->textureId(), QOpenGLTexture::DontResetTextureUnit);
            it.value()->setUniform(_program, current->textureId());
        }
    }

    QOpenGLShaderProgram * AbstractModel::program() const {
        return _program;
    }

    bool AbstractModel::updateNeeded() const {
        return _updateNeeded;
    }

    void AbstractModel::queueForUpdate() {
        _updateNeeded = true;
    }

    void AbstractModel::drawModel(const Viewport::Viewport * viewport) {
        /* model can contain no vertices or | and no program
        so it can serve as a "root" model, containing some
        number of children: models with vertices and program.
        So children can share some common properties
        */
        if (_program && _vertexCount) {
            bindShaderProgram();

            bindUniformValues(_program, viewport);
            bindUniformValues();
            
            processTextures(&QOpenGLTexture::bind);
            
            glStatesEnable();
            
            _vao.bind();
            
            drawingRoutine();
            
            glFinish();
            _vao.release();
            
            processTextures(&QOpenGLTexture::release);
            glStatesDisable();
            releaseShaderProgram();
        }
    }

    void AbstractModel::drawingRoutine() const {
        QMutexLocker locker (&modelMutex);
        if (_indexCount) {
            glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
        }
    }

    void AbstractModel::bindUniformValues() const {
        QMutexLocker locker (&modelMutex);

        QMapIterator<Scene::Material *, Scene::MaterialProgram *> itM (_materials);

        while (itM.hasNext()) {
            itM.next();
            itM.value()->setUniform(_program, itM.key());
        }

        QMapIterator<Scene::LightSource *, Scene::LightProgram *> itL (_lightSources);

        while (itL.hasNext()) {
            itL.next();
            itL.value()->setUniformValue(_program, itL.key());
        }
    }

    bool AbstractModel::initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles) {
        QMutexLocker locker (&modelMutex);

        bool programIsInited = true;

        if (!_program) {
            _program = new QOpenGLShaderProgram;

            for (const ShaderInfo::VertexShaderFile & vertex : shaderFiles.vertexShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex);
            }

            for (const ShaderInfo::FragmentShaderFile & fragment : shaderFiles.fragmentShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment);
            }

            for (const ShaderInfo::GeometryShaderFile & geometry : shaderFiles.geometryShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry);
            }

            if (!shaderFiles.tesselationEvaluationShaderFiles.empty()) {
                for (const ShaderInfo::TesselationEvaluationShaderFile & tessEval : shaderFiles.tesselationEvaluationShaderFiles) {
                    programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, tessEval);
                }

                for (const ShaderInfo::TesselationControlShaderFile & tessControl : shaderFiles.tesselationControlShaderFiles) {
                    programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationControl, tessControl);
                }
            }

            programIsInited &= _program->link();

            if (!programIsInited) {
                return false;
            }

            initShaderVariables();
        }
        else {
            return !programIsInited;
        }

        return programIsInited;
    }

    void AbstractModel::initShaderVariables() {
        for (ShaderInfo::ShaderVariableName attrArray : attributeArrays.keys()) {
            attributeArrays[attrArray] = _program->attributeLocation(attrArray);
        }

        for (ShaderInfo::ShaderVariableName value : uniformValues.keys()) {
            uniformValues[value] = _program->uniformLocation(value);
        }
    }

    void AbstractModel::update() {
        bindShaderProgram();

        updateRoutine();

        releaseShaderProgram();

        _updateNeeded = false;
    }

    void AbstractModel::updateRoutine() {

    }

    void AbstractModel::glStatesEnable() const {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glStencilFunc(GL_ALWAYS, _numberedID + 1, -1);
    }

    void AbstractModel::glStatesDisable() const {
        glDisable(GL_STENCIL_TEST);
    }

    void AbstractModel::invoke(const QString & name, const ModelInfo::Params & params) {
        if (name == "rotate") {
            if (params.contains("speed")) {
                rotate(params["angle"].value<QVector3D>(), params["speed"].toReal());
            }
            else {
                rotate(params["angle"].value<QVector3D>());
            }
            return;
        }

        if (name == "translate") {
            translate(params["translation"].value<QVector3D>());
            return;
        }

        if (name == "scale") {
            scale(params["scale"].value<QVector3D>());
        }
    }

    QList<AbstractModel *> AbstractModel::childModels() const {
        return _children;
    }
}
