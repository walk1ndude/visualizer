#include "Model/AbstractModel.h"

namespace Model {
    AbstractModel::AbstractModel(AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        _vboVert(QOpenGLBuffer::VertexBuffer),
        _vboInd(QOpenGLBuffer::IndexBuffer),
        _program(nullptr),
        _shaderFiles(shaderFiles),
        _stride(0),
        _indexCount(0),
        _vertexCount(0),
        _parent(parent) {
        if (_parent) {
            _parent->addChild(this);
        }
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

    void AbstractModel::setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                     const ModelInfo::ViewAxisRange & yRange,
                                     const ModelInfo::ViewAxisRange & zRange,
                                     const ShaderInfo::ShaderVariablesNames & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (_program) {
            _viewRange = new ModelInfo::ViewRange(correctedViewwAxisRange(xRange),
                                                  correctedViewwAxisRange(yRange),
                                                  correctedViewwAxisRange(zRange),
                                                  _program, shaderVariables);
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
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

    void AbstractModel::addChild(AbstractModel * child) {
        _children.append(child);
    }

    QMatrix4x4 AbstractModel::model() {
        return _mMatrix;
    }

    bool AbstractModel::checkDepthBuffer(ViewPort::ViewPort & viewPort) {
        Q_UNUSED(viewPort);
        return false;
    }

    ModelInfo::ViewAxisRange AbstractModel::correctedViewwAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange) {
        return viewAxisRange;
    }

    void AbstractModel::rotate(const QVector3D & rotation) {
        QVector3D rot = _orientation - rotation;

        _mMatrix.rotate(QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rot.x()) *
                        QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rot.y()) *
                        QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rot.z())
        );

        _orientation = rotation;
    }

    void AbstractModel::setViewAxisRange(const ModelInfo::ViewAxisRange & viewAxisRange,
                                         const ModelInfo::ViewAxis viewAxis) {
        _viewRange->setViewAxisRange(correctedViewwAxisRange(viewAxisRange), viewAxis);
    }

    bool AbstractModel::bindShaderProgram() {
        return _program->bind();
    }

    void AbstractModel::releaseShaderProgram() {
        return _program->release();
    }

    void AbstractModel::bindTextures() {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);
        uint samplerNum = 0;

        while (it.hasNext()) {
            it.next();
            it.key()->bind(samplerNum);
            it.value()->setUniform(_program, samplerNum);

            ++ samplerNum;
        }
    }

    void AbstractModel::releaseTextures() {
        QMapIterator<QOpenGLTexture *, TextureInfo::TextureProgram *> it (_textures);
        uint samplerNum = 0;

        while (it.hasNext()) {
            it.next().key()->release(samplerNum ++);
        }
    }

    uint AbstractModel::modelID() {
        return _modelID;
    }

    void AbstractModel::drawModel(ViewPort::ViewPort & viewPort) {
        if (_program) {
            bindShaderProgram();
            
            glStatesEnable();
            
            setShaderVariables(_program, viewPort);
            setShaderVariables();
            
            bindTextures();
            
            _vao.bind();
            
            if (_indexCount) {
                drawModelWithIndices();
            }
            else {
                drawModelWithoutIndices();
            }
            
            glFinish();
            _vao.release();
            
            if (checkDepthBuffer(viewPort)) {
                emit redraw();
            }
            
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

        _viewRange->setUniformValue(_program);
    }

    bool AbstractModel::initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles) {
        bool programIsInited = true;

        if (!_program) {
            _program = new QOpenGLShaderProgram;

            foreach (const ShaderInfo::VertexShaderFile & vertex, shaderFiles.vertexShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex);
            }

            foreach (const ShaderInfo::FragmentShaderFile & fragment, shaderFiles.fragmentShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment);
            }

            foreach (const ShaderInfo::GeometryShaderFile & geometry, shaderFiles.geometryShaderFiles) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Geometry, geometry);
            }

            if (!shaderFiles.tesselationEvaluationShaderFiles.empty()) {
                foreach (const ShaderInfo::TesselationEvaluationShaderFile & tessEval, shaderFiles.tesselationEvaluationShaderFiles) {
                    programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, tessEval);
                }

                foreach (const ShaderInfo::TesselationControlShaderFile & tessControl, shaderFiles.tesselationControlShaderFiles) {
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
