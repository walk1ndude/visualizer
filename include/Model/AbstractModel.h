#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H

#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLTexture>

#include "Info/Modelinfo.h"
#include "Info/Materialinfo.h"
#include "Info/LightInfo.h"
#include "Info/ShaderInfo.h"

namespace Model {
    class AbstractModel : public QObject {
        Q_OBJECT
    public:
        explicit AbstractModel(const ShaderInfo::ShaderFiles & shaderFiles);
        virtual ~AbstractModel();

        virtual void initModel(ModelInfo::BuffersV & buffers) = 0;

        virtual void initShaderVariables() = 0;

        virtual bool bindShaderProgram() final;
        virtual void releaseShaderProgram() final;

        virtual void drawModel() final;

        virtual void addMaterial(MaterialInfo::Material * material, const QStringList & shaderVariables) final;
        virtual void addLightSource(LightInfo::LightSource * lightSource, const QStringList & shaderVariables) final;

        virtual void setTexelRange(const ModelInfo::TexelRange & texelRange,
                                   const QStringList & shaderVariables) final;

        virtual void setTexelRange(const ModelInfo::TexelAxisRange & sRange,
                                   const ModelInfo::TexelAxisRange & tRange,
                                   const ModelInfo::TexelAxisRange & pRange,
                                   const QStringList & shaderVariables) final;

        virtual void setTexelAxisRange(const ModelInfo::TexelAxisRange & texelAxisRange,
                                       const ModelInfo::TexelAxis texelAxis = ModelInfo::SAXIS) final;

        virtual uint modelID() final;

    protected:
        QOpenGLBuffer _vboVert;
        QOpenGLBuffer _vboInd;

        QOpenGLVertexArrayObject _vao;

        QOpenGLShaderProgram * _program;

        GLsizei _indexCount;
        GLsizei _vertexCount;

        QMap<MaterialInfo::Material *, MaterialInfo::MaterialProgram *> _materials;
        QMap<LightInfo::LightSource *, LightInfo::LightProgram *> _lightSources;

        ModelInfo::TexelRange * _texelRange;

        QMutex _modelMutex;

        ShaderInfo::ShaderFiles _shaderFiles;

    private:
        uint _modelID;

        bool _hasIndices;

        bool initShaderProgram(const ShaderInfo::ShaderFiles & shaderFiles);
        void setShaderVariables();

        void drawModelWithIndices();
        void drawModelWithoutIndices();

    signals:
        void shaderProgramInitErrorHappened();
        void shaderProgramSetVariableErrorHappened();

    public slots:
        virtual void createModel(ModelInfo::BuffersV & buffers,
                                 const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::StaticDraw) final;

        virtual void updateModel(ModelInfo::BuffersV & buffers,
                                 const QOpenGLBuffer::UsagePattern usagePattern = QOpenGLBuffer::UsagePattern::DynamicDraw) final;

        virtual void deleteModel() final;
    };
}

#endif // ABSTRACTMODEL_H
