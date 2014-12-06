#include "Model/PointsModel.h"
#include "Model/VertexVC.h"

namespace Model {
    PointsModel::PointsModel(Scene::AbstractScene * scene,
                             const ShaderInfo::ShaderFiles & shaderFiles,
                             const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                             const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModel(scene, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        lockToModelAxis();
    }

    void PointsModel::init(const ModelInfo::Params & params) {
        PointsInfo::ModelPoints * modelPoints = params["modelPoints"].value<PointsInfo::ModelPoints *>();

        ModelInfo::VerticesVCPtr vertices = new ModelInfo::VerticesVC;

        if (modelPoints) {
            ModelInfo::VertexVC vertex;

            QHash<QString, int>groups;
            int pos = 0;

            for (const PointsInfo::ModelPoint * modelPoint : modelPoints->points()) {
                vertex = ModelInfo::VertexVC(modelPoint->position.x(),
                                             modelPoint->position.y(),
                                             modelPoint->position.z(),
                                             modelPoint->color.redF(),
                                             modelPoint->color.greenF(),
                                             modelPoint->color.blueF());

                if (modelPoint->groups.isEmpty()) {
                    /* if point belongs to "ungrouped" build a "point" triangle:
                     * p_a = p_b = p_c */
                    pos += 3;

                    for (int i = 0; i != 3; ++ i) {
                        vertices->push_back(vertex);
                    }

                    continue;
                }

                for (const QString & group : modelPoint->groups) {
                    /* loop through groups to which the point belongs:
                     * if it's the first time for group with
                     * this name - build a "point" triangle,
                     * otherwise try to fill it. If group has more
                     * than 3 vertices, build a new triangle with 2
                     * known vertices and 1 new */
                    if (!groups[group]) {
                        groups[group] = pos + 1;
                        pos += 3;

                        for (int i = 0; i != 3; ++ i) {
                            vertices->push_back(vertex);
                        }
                    }
                    else {
                        int pos3 = groups[group];

                        if (groups[group] % 3 != 0) {
                            (*vertices)[groups[group]] = vertex;
                            groups[group] ++;
                        }
                        else {
                            vertices->push_back(vertices->at(pos3 - 2));
                            vertices->push_back(vertices->at(pos3 - 1));
                            vertices->push_back(vertex);

                            pos += 3;
                        }
                    }
                }
            }
        }

        ModelInfo::BuffersVC buffers;
        buffers.vertices = ModelInfo::VerticesVCPointer(vertices);

        AbstractModel::fillBuffers<ModelInfo::BuffersVC>(buffers, QOpenGLBuffer::DynamicDraw);
    }

    void PointsModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["color"]);
        program->setAttributeBuffer(attributeArrays["color"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void PointsModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["mvp"], parent()->childsMVP(viewport, this));

        program->setUniformValue(uniformValues["viewportSize"], QVector4D(viewport->width(), viewport->height(), 0.0f, 0.0f));
    }

    void PointsModel::glStatesEnable() const {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        AbstractModel::glStatesEnable();
    }

    void PointsModel::glStatesDisable() const {
        glDisable(GL_DEPTH_TEST);

        AbstractModel::glStatesDisable();
    }

    bool PointsModel::hasDepth() const {
        return false;
    }

    void PointsModel::invoke(const QString & name, const ModelInfo::Params & params) {
        if (name == "init") {
            init(params);
        }
    }
}

REGISTER_TYPE(PointsModel)
