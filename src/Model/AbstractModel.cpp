#include "Model/AbstractModel.h"

namespace Model {
    AbstractModel::AbstractModel() :
        _vboVert(QOpenGLBuffer::VertexBuffer),
        _vboInd(QOpenGLBuffer::IndexBuffer),
        _program(0),
        _indexCount(0),
        _vertexCount(0) {
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

        QMapIterator<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> itM (_materials);

        while (itM.hasNext()) {
            delete itM.next().value();
        }

        _materials.clear();

        QMapIterator<LightInfo::LightSource *, LightInfo::LightProgram *> itL (_lightSources);

        while (itL.hasNext()) {
            delete itL.next().value();
        }

        _lightSources.clear();
    }

    void AbstractModel::addMaterial(MaterialInfo::Material * material, const QStringList & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (_program) {
            _materials.insert(material, new MaterialInfo::MaterialProgram(_program, shaderVariables));
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
    }

    void AbstractModel::addLightSource(LightInfo::LightSource * lightSource, const QStringList & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (_program) {
            _lightSources.insert(lightSource, new LightInfo::LightProgram(_program, shaderVariables));
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
    }

    void AbstractModel::setTexelRange(const ModelInfo::TexelRange & texelRange,
                                      const QStringList & shaderVariables) {
        setTexelRange(texelRange.sRange, texelRange.tRange, texelRange.pRange, shaderVariables);
    }

    void AbstractModel::setTexelRange(const ModelInfo::TexelAxisRange & sRange,
                                      const ModelInfo::TexelAxisRange & tRange,
                                      const ModelInfo::TexelAxisRange & pRange,
                                      const QStringList & shaderVariables) {
        QMutexLocker locker(&_modelMutex);

        if (_program) {
            if (_texelRange) {
                delete _texelRange;
            }
            _texelRange = new ModelInfo::TexelRange(sRange, tRange, pRange, _program, shaderVariables);
        }
        else {
            emit shaderProgramSetVariableErrorHappened();
        }
    }

    void AbstractModel::setTexelAxisRange(const ModelInfo::TexelAxisRange & texelAxisRange,
                           const ModelInfo::TexelAxis texelAxis) {
        _texelRange->setTexelAxisRange(texelAxisRange, texelAxis);
    }

    bool AbstractModel::bindShaderProgram() {
        return _program->bind();
    }

    void AbstractModel::releaseShaderProgram() {
        return _program->release();
    }

    uint AbstractModel::modelID() {
        return _modelID;
    }

    void AbstractModel::drawModel() {
        if (_hasIndices) {
            drawModelWithoutIndices();
        }
        else {
            drawModelWithoutIndices();
        }
    }

    void AbstractModel::drawModelWithIndices() {
        bindShaderProgram();
        setShaderVariables();

        _vao.bind();

        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0);

        _vao.release();

        releaseShaderProgram();
    }

    void AbstractModel::drawModelWithoutIndices() {
        bindShaderProgram();
        setShaderVariables();

        _vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, _vertexCount);

        _vao.release();

        releaseShaderProgram();
    }

    void AbstractModel::setShaderVariables() {
        QMutexLocker locker(&_modelMutex);

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

        _texelRange->setUniformValue(_program);
    }

    bool AbstractModel::initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles) {
        bool programIsInited = true;

        if (!_program) {
            _program = new QOpenGLShaderProgram;

            programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Vertex, shaderFiles.vertexShaderFile);
            programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Fragment, shaderFiles.framentShaderFile);

            if (shaderFiles.geometryShaderFile) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::Geometry, shaderFiles.geometryShaderFile);
            }

            if (shaderFiles.tesselationEvaluationShaderFile) {
                programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationEvaluation, shaderFiles.tesselationEvaluationShaderFile);

                if (shaderFiles.tesselationControlShaderFile) {
                    programIsInited &= _program->addShaderFromSourceFile(QOpenGLShader::TessellationControl, shaderFiles.tesselationControlShaderFile);
                }
            }

            if (!programIsInited) {
                return false;
            }

            programIsInited &= _program->link();

            _shaderModel = _program->uniformLocation("model");
            _shaderView = _program->uniformLocation("view");
            _shaderProjection = _program->uniformLocation("projection");
            _shaderScale = _program->uniformLocation("scale");

            initShaderVariables();
        }
        else {
            return false;
        }

        return true;
    }

    void AbstractModel::createModel(const ShaderInfo::ShaderFiles & shaderFiles, ModelInfo::BuffersV & buffers,
                                    const QOpenGLBuffer::UsagePattern usagePattern) {
        if (!initShaderProgram(shaderFiles)) {
            emit shaderProgramInitErrorHappened();
            return;
        }

        updateModel(buffers, usagePattern);
    }

    void AbstractModel::updateModel(ModelInfo::BuffersV & buffers, const QOpenGLBuffer::UsagePattern usagePattern) {
        bindShaderProgram();

        _vao.create();
        _vao.bind();

        _vboVert.create();
        _vboVert.setUsagePattern(usagePattern);

        _vertexCount = buffers.vertices.data()->size();

        _vboVert.bind();
        _vboVert.allocate(buffers.vertices.data(), _vertexCount * sizeof(buffers.vertices->at(0)));

        if (buffers.indices.data()) {
            _vboInd.create();
            _vboInd.setUsagePattern(usagePattern);

            _indexCount = buffers.indices.data()->size();

            _vboInd.bind();
            _vboInd.allocate(buffers.indices.data(), _indexCount * sizeof(GLushort));

            buffers.indices.clear();

            _hasIndices = true;
        }
        else {
            _hasIndices = false;
        }

        initModel(buffers);

        _vao.release();

        releaseShaderProgram();

        buffers.vertices.clear();
    }
}
