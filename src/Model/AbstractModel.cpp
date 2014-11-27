#include "Model/AbstractModel.h"

#include <cmath>

static uint modelNumber = 0;

namespace Model {
    QHash<ModelInfo::Type, ModelFactory *> AbstractModel::_factories;

    AbstractModel::AbstractModel(Scene::AbstractScene * scene,
                                 AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles,
                                 const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                                 const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        _vboVert(QOpenGLBuffer::VertexBuffer),
        _vboInd(QOpenGLBuffer::IndexBuffer),
        _program(nullptr),
        _scene(scene),
        _stride(0),
        _depthTest(true),
        _indexCount(0),
        _vertexCount(0),
        _updateNeeded(false),
        _lockToWorldAxis(true) {

        if (_parent) {
            QObject::disconnect(this, &AbstractModel::childUpdated, _parent, &AbstractModel::update);
        }

        _parent = parent;

        if (_parent) {
            _parent->addChild(this);
            QObject::connect(this, &AbstractModel::childUpdated, _parent, &AbstractModel::update);
        }

        _id = modelNumber ++;

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
        initFromParams<LightInfo::LightSource *, LightInfo::LightID>(
                    params["lights"].toMap(),
                    LightInfo::LightSource::initializationOrder,
                    &AbstractModel::addLightSource,
                    _scene, &Scene::AbstractScene::lightSource
                );

        initFromParams<MaterialInfo::Material *, MaterialInfo::MaterialID>(
                    params["materials"].toMap(),
                    MaterialInfo::Material::initializationOrder,
                    &AbstractModel::addMaterial,
                    _scene, &Scene::AbstractScene::material
                    );
    }

    void AbstractModel::registerType(const ModelInfo::Type & name, ModelFactory * factory) {
        _factories[name] = factory;
    }

    AbstractModel * AbstractModel::createModel(const ModelInfo::Type & name, Scene::AbstractScene * scene,
                                               AbstractModel * parent) {
        return _factories[name]->createModel(scene, parent);
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

        qDeleteAll(_materials.begin(), _materials.end());
        _materials.clear();

        qDeleteAll(_lightSources.begin(), _lightSources.end());
        _lightSources.clear();
    }

    void AbstractModel::addMaterial(MaterialInfo::Material * material, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram>(_materials, material, shaderVariables);
    }

    void AbstractModel::addLightSource(LightInfo::LightSource * lightSource, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<LightInfo::LightSource *, LightInfo::LightProgram>(_lightSources, lightSource, shaderVariables);
    }

    void AbstractModel::addTexture(QOpenGLTexture * texture, const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        addToMap<QOpenGLTexture *, TextureInfo::TextureProgram>(_textures, texture, shaderVariables);
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
        child->setParent(this);
        _children.append(child);
    }

    Camera::ModelMatrix AbstractModel::model(const Viewport::Viewport *) const {
        Camera::ModelMatrix mMatrix;
        mMatrix.translate(_position);
        mMatrix.rotate(_orientation);

        return mMatrix;
    }

    Camera::Matrix AbstractModel::childsMVP(const Viewport::Viewport * viewport, const AbstractModel * ) const {
        return mvp(viewport);
    }

    Camera::Matrix AbstractModel::mvp(const Viewport::Viewport * viewport) const {
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
        return viewport->view();
    }

    Camera::ProjectionMatrix AbstractModel::projection(const Viewport::Viewport * viewport) const {
        return viewport->projection();
    }
    
    Camera::ViewMatrix AbstractModel::lightView(const Viewport::Viewport * viewport) const {
        return viewport->view();
    }

    Camera::NormalMatrix AbstractModel::normalMatrix(const Viewport::Viewport * viewport) const {
        return model(viewport).normalMatrix();
    }

    Camera::ScaleMatrix AbstractModel::scaleMatrix() const {
        return _scaleM;
    }

    bool AbstractModel::checkDepthBuffer(const Viewport::Viewport * ) {
        return false;
    }

    void AbstractModel::translate(const QVector3D & translation) {
        _position += translation;
    }

    void AbstractModel::scale(const QVector3D & scale) {
        if (_scale.isNull()) {
            _scale = scale;
        }
        else {
            _scale *= scale;
        }

        _scaleM.scale(_scale);
    }

    void AbstractModel::lockToModelAxis() {
        _lockToWorldAxis = false;
    }

    void AbstractModel::lockToWorldAxis() {
        _lockToWorldAxis = true;
    }

    void AbstractModel::rotate(const QVector3D & rotation, const qreal & speed) {
        _orientation = changedOrientation(
            Camera::Rotation::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rotation.x() * speed) *
            Camera::Rotation::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rotation.y() * speed) *
            Camera::Rotation::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rotation.z() * speed)
                                          );

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

    void AbstractModel::bindTextures() const {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);

        while (it.hasNext()) {
            it.next();
            it.key()->bind(it.key()->textureId());
            it.value()->setUniform(_program, it.key()->textureId());
        }
    }

    void AbstractModel::releaseTextures() const {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);

        while (it.hasNext()) {
            it.next();
            it.key()->release(it.key()->textureId());
        }
    }

    uint AbstractModel::id() const {
        return _id;
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
            
            bindTextures();
            
            glStatesEnable();
            
            _vao.bind();
            
            drawingRoutine();
            
            glFinish();
            _vao.release();
            
            releaseTextures();
            glStatesDisable();
            releaseShaderProgram();
        }
    }

    void AbstractModel::drawingRoutine() const {
        if (_indexCount) {
            glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
        }
    }

    void AbstractModel::bindUniformValues() const {
        QMapIterator<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> itM (_materials);

        while (itM.hasNext()) {
            itM.next();
            itM.value()->setUniform(_program, itM.key());
        }

        QMapIterator<LightInfo::LightSource *, LightInfo::LightProgram *> itL (_lightSources);

        while (itL.hasNext()) {
            itL.next();
            itL.value()->setUniformValue(_program, itL.key());
        }
    }

    bool AbstractModel::initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles) {
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

        glStencilFunc(GL_ALWAYS, id() + 1, -1);
    }

    void AbstractModel::glStatesDisable() const {
        glDisable(GL_STENCIL_TEST);
    }

    bool AbstractModel::hasDepth() const {
        return true;
    }

    void AbstractModel::addPoint(const PointsInfo::Name & name, PointsInfo::ModelPoint * point) {
        Q_UNUSED(name)
        Q_UNUSED(point)
    }

    void AbstractModel::togglePoint(const PointsInfo::Name & point) {
        Q_UNUSED(point)
    }

    void AbstractModel::invoke(const QString & name, const ModelInfo::Params & params) {
        if (name == "rotate") {
            if (params.contains("speed")) {
                rotate(params["rotation"].value<QVector3D>(), params["speed"].toReal());
            }
            else {
                rotate(params["rotation"].value<QVector3D>());
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
}
