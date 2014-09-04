#include "Model/PointsModel.h"
#include "Model/HeadModel.h"
#include "Model/StlModel.h"

namespace Model {
    PointsModel::PointsModel(AbstractModel * parent, const ShaderInfo::ShaderFiles & shaderFiles) :
        AbstractModel(parent, shaderFiles) {

    }

    void PointsModel::fillBuffers(const PointsInfo::ModelPoints & modelPoints) {
        if (updateNeeded()) {
            ModelInfo::VertexVC vertex;
            ModelInfo::VerticesVCPtr vertices = new ModelInfo::VerticesVC;

            QHash<QString, int>groups;
            int pos = 0;

            for (const PointsInfo::ModelPoint * modelPoint : modelPoints) {
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
                     * know vertices and 1 new */
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

            for (int i = 0; i < vertices->length(); ++ i) {
                qDebug() << vertices->at(i).x << vertices->at(i).y << vertices->at(i).z;
            }

            ModelInfo::BuffersVC buffers;
            buffers.vertices = ModelInfo::VerticesVCPointer(vertices);

            AbstractModel::fillBuffers<ModelInfo::BuffersVC>(buffers, QOpenGLBuffer::DynamicDraw);
        }
    }

    void PointsModel::glStatesEnable() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void PointsModel::glStatesDisable() {
        glDisable(GL_DEPTH_TEST);
    }

    void PointsModel::initShaderVariables(QOpenGLShaderProgram * program) {
        _shaderVertex = program->attributeLocation("vertex");
        _shaderColor = program->attributeLocation("color");

        _shaderMVP = program->uniformLocation("mvp");

        _shaderViewportSize = program->uniformLocation("viewportSize");
    }

    void PointsModel::bindShaderVariablesToBuffers(QOpenGLShaderProgram * program) {
        program->enableAttributeArray(_shaderVertex);
        program->setAttributeBuffer(_shaderVertex, GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(_shaderColor);
        program->setAttributeBuffer(_shaderColor, GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void PointsModel::setShaderVariables(QOpenGLShaderProgram * program, Viewport::Viewport * viewport) {
        program->setUniformValue(_shaderMVP, projection(viewport) * parent()->view(viewport) * parent()->model(viewport));

        program->setUniformValue(_shaderViewportSize, QVector4D(viewport->width(), viewport->height(), 0.0f, 0.0f));
    }
}
