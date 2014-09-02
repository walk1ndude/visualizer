#include "Model/AbstractModel.h"

static int modelNumber = 0;

static float normalizedAngle(const float & angle) {
    float normalized = angle;

    while (normalized < 0.0f) {
        normalized += 360.0f;
    }
    while (normalized > 360.0f) {
        normalized -= 360.0f;
    }

    return normalized;
}

namespace Model {
    AbstractModel::AbstractModel(AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        _vboVert(QOpenGLBuffer::VertexBuffer),
        _vboInd(QOpenGLBuffer::IndexBuffer),
        _program(nullptr),
        _shaderFiles(shaderFiles),
        _stride(0),
        _indexCount(0),
        _vertexCount(0),
        _updateNeeded(false) {

        if (_parent) {
            QObject::disconnect(this, &AbstractModel::childUpdated, _parent, &AbstractModel::update);
        }

        _parent = parent;

        if (_parent) {
            _parent->addChild(this);
            QObject::connect(this, &AbstractModel::childUpdated, _parent, &AbstractModel::update);
        }

        _id = modelNumber ++;
    }

    AbstractModel::~AbstractModel() {
        deleteModel();
    }

    void AbstractModel::deleteModel() {
        QMutexLocker locker(&_modelMutex);

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

    int AbstractModel::stride() {
        return _stride;
    }

    GLsizei AbstractModel::vertexCount() {
        return _vertexCount;
    }

    GLsizei AbstractModel::indexCount() {
        return _indexCount;
    }

    AbstractModel * AbstractModel::parent() {
        return _parent;
    }
    
    void AbstractModel::setParent(AbstractModel * parent) {
        _parent = parent;
    }

    void AbstractModel::addChild(AbstractModel * child) {
        _children.append(child);
    }

    QMatrix4x4 AbstractModel::model(Viewport::Viewport * ) {
        return _mMatrix;
    }

    QMatrix4x4 AbstractModel::view(Viewport::Viewport * viewport) {
        return viewport->view();
    }

    QMatrix4x4 AbstractModel::projection(Viewport::Viewport * viewport) {
        return viewport->projection();
    }

    QMatrix3x3 AbstractModel::normalMatrix(Viewport::Viewport * viewport) {
        return (model(viewport) * view(viewport)).normalMatrix();
    }

    bool AbstractModel::checkDepthBuffer(Viewport::Viewport * ) {
        return false;
    }

    void AbstractModel::rotate(const QVector3D & rotation) {
        _mMatrix.rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, normalizedAngle(rotation.x())) *
                        QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, normalizedAngle(rotation.y())) *
                        QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, normalizedAngle(rotation.z()))
        );
    }

    bool AbstractModel::bindShaderProgram() {
        return _program->bind();
    }

    void AbstractModel::releaseShaderProgram() {
        _program->release();
    }

    void AbstractModel::bindTextures() {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);

        while (it.hasNext()) {
            it.next();
            it.key()->bind(it.key()->textureId());
            it.value()->setUniform(_program, it.key()->textureId());
        }
    }

    void AbstractModel::releaseTextures() {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);

        while (it.hasNext()) {
            it.next().key()->release(it.key()->textureId());
        }
    }

    int AbstractModel::id() {
        return _id;
    }

    QOpenGLShaderProgram * AbstractModel::program() {
        return _program;
    }

    bool AbstractModel::updateNeeded() {
        return _updateNeeded;
    }

    void AbstractModel::queueForUpdate() {
        _updateNeeded = true;
    }

    void AbstractModel::drawModel(Viewport::Viewport * viewPort) {
        /* model can contain no vertices or | and no program
        so it can serve as a "root" model, containing some
        number of children: models with vertices and program.
        So children can share some common properties
        */
        if (_program && _vertexCount) {
            bindShaderProgram();
            
            setShaderVariables(_program, viewPort);
            setShaderVariables();
            
            bindTextures();
            
            glStatesEnable();
            
            _vao.bind();
            
            if (_indexCount) {
                drawModelWithIndices();
            }
            else {
                drawModelWithoutIndices();
            }
            
            glFinish();
            _vao.release();
            
            releaseTextures();
            glStatesDisable();
            releaseShaderProgram();
        }
    }

    void AbstractModel::drawModelWithIndices() {
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);
    }

    void AbstractModel::drawModelWithoutIndices() {
        glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
    }

    void AbstractModel::setShaderVariables() {
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

            initShaderVariables(_program);
        }
        else {
            return !programIsInited;
        }

        return programIsInited;
    }
}
