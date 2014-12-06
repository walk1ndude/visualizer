#include "Model/AxesModel.h"
#include "Model/VertexVC.h"

namespace Model {
    AxesModel::AxesModel(Scene::AbstractScene * scene,
                         const ShaderInfo::ShaderFiles & shaderFiles,
                         const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                         const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
    AbstractModel(scene, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        //lockToModelAxis();
    }

    void AxesModel::init(const ModelInfo::Params & params) {
        AbstractModel::init(params);

        QVariantMap colorMap = params["axesColor"].toMap();
        QVector<QColor> colors;

        QString helper;

        QStringList colorHashMap = {"x", "y", "z"};

        for (const QString & color : colorHashMap) {
            helper = colorMap[color].toString();

            if (helper.isEmpty()) {
                colors << colorMap[color].value<QColor>();
            }
            else {
                colors << QColor(helper);
            }
        }

        setColors(colors);
        setLenght(params["length"].toReal());

        ModelInfo::VerticesVCPtr vertices = new ModelInfo::VerticesVC;

        GLfloat length = 1.5;

        vertices->push_back(ModelInfo::VertexVC(0.0f, 0.0f, 0.0f,
                                                _axesColors[0].redF(),
                                                _axesColors[0].greenF(),
                                                _axesColors[0].blueF()));
        vertices->push_back(ModelInfo::VertexVC(length, 0.0f, 0.0f,
                                               _axesColors[0].redF(),
                                               _axesColors[0].greenF(),
                                               _axesColors[0].blueF()));

        vertices->push_back(ModelInfo::VertexVC(0.0f, 0.0f, 0.0f,
                                               _axesColors[1].redF(),
                                               _axesColors[1].greenF(),
                                               _axesColors[1].blueF()));
        vertices->push_back(ModelInfo::VertexVC(0.0f, length, 0.0f,
                                               _axesColors[1].redF(),
                                               _axesColors[1].greenF(),
                                               _axesColors[1].blueF()));

        vertices->push_back(ModelInfo::VertexVC(0.0f, 0.0f, 0.0f,
                                               _axesColors[2].redF(),
                                               _axesColors[2].greenF(),
                                               _axesColors[2].blueF()));
        vertices->push_back(ModelInfo::VertexVC(0.0f, 0.0f, length,
                                               _axesColors[2].redF(),
                                               _axesColors[2].greenF(),
                                               _axesColors[2].blueF()));

        ModelInfo::BuffersVC buffers;

        buffers.vertices = ModelInfo::VerticesVCPointer(vertices);

        AbstractModel::fillBuffers<ModelInfo::BuffersVC>(buffers);
    }

    void AxesModel::setColors(const QVector<QColor> & axesColors) {
        _axesColors = axesColors;
    }

    void AxesModel::setLenght(const GLfloat & lenght) {
        _lenght = lenght;
    }

    void AxesModel::drawingRoutine() const {
        glDrawArrays(GL_LINES, 0, vertexCount());
    }

    void AxesModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["color"]);
        program->setAttributeBuffer(attributeArrays["color"], GL_FLOAT, sizeof(GLfloat) * 3, 4, stride());
    }

    void AxesModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        Camera::Matrix mvp;
        if (parent()) {
            mvp = parent()->childsMVP(viewport, this);
        }
        else {
            mvp = projection(viewport) * view(viewport) * model(viewport);
        }

        program->setUniformValue(uniformValues["mvp"], mvp);
    }

    void AxesModel::glStatesEnable() const {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        AbstractModel::glStatesEnable();
    }

    void AxesModel::glStatesDisable() const {
        glDisable(GL_DEPTH_TEST);

        AbstractModel::glStatesDisable();
    }
}

REGISTER_TYPE(AxesModel)
