#include "Model/AbstractModel.h"

namespace Model {
    AbstractModel::AbstractModel(const ShaderInfo::ShaderFiles & shaderFiles) :
        _vboVert(QOpenGLBuffer::VertexBuffer),
        _vboInd(QOpenGLBuffer::IndexBuffer),
        _program(nullptr),
        _indexCount(0),
        _vertexCount(0),
        _shaderFiles(shaderFiles) {

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

    void AbstractModel::addMaterial(MaterialInfo::Material * material, const ShaderInfo::ShaderVariables & shaderVariables) {
        addToMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram>(_materials, material, shaderVariables);
    }

    void AbstractModel::addLightSource(LightInfo::LightSource * lightSource, const ShaderInfo::ShaderVariables & shaderVariables) {
        addToMap<LightInfo::LightSource *, LightInfo::LightProgram>(_lightSources, lightSource, shaderVariables);
    }

    void AbstractModel::addTexture(QOpenGLTexture * texture, const ShaderInfo::ShaderVariables & shaderVariables) {
        addToMap<QOpenGLTexture *, TextureInfo::TextureProgram>(_textures, texture, shaderVariables);
    }

    void AbstractModel::setViewRange(const ModelInfo::ViewAxisRange & xRange,
                                     const ModelInfo::ViewAxisRange & yRange,
                                     const ModelInfo::ViewAxisRange & zRange,
                                     const ShaderInfo::ShaderVariables & shaderVariables) {
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
        //QMutexLocker locker(&_modelMutex);

        bindShaderProgram();

        glStatesEnable();

        setShaderVariables(viewPort);
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

        releaseTextures();
        glStatesDisable();
        releaseShaderProgram();
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

            programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Vertex, shaderFiles.vertexShaderFile);
            programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Fragment, shaderFiles.framentShaderFile);

            if (shaderFiles.geometryShaderFile.length()) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Geometry, shaderFiles.geometryShaderFile);
            }

            if (shaderFiles.tesselationEvaluationShaderFile.length()) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, shaderFiles.tesselationEvaluationShaderFile);

                if (shaderFiles.tesselationControlShaderFile.length()) {
                    programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationControl, shaderFiles.tesselationControlShaderFile);
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
}
