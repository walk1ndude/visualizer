#include "Model/VolumeModel.h"

namespace Model {
    VolumeModel::VolumeModel(Scene::AbstractScene * scene, AbstractModel * parent,
                         const ShaderInfo::ShaderFiles & shaderFiles,
                         const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                         const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModelWithPoints(scene, parent, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        lockToModelAxis();
    }

    void VolumeModel::init(const TextureInfo::Size & size, const VolumeInfo::PhysicalSize & physicalSize,
                         const TextureInfo::Scaling & scaling) {
        ModelInfo::VerticesVTPtr vertices = new ModelInfo::VerticesVT;
        ModelInfo::IndicesPtr indices = new ModelInfo::Indices;

        GLfloat scalingFactor = (GLfloat) scene()->scalingFactor();

        GLfloat zCurrent = - physicalSize.z() * scaling.z() / scalingFactor / 2.0f;
        
        GLfloat step = - (zCurrent * 2.0f) / size.z();
        GLfloat stepTexture = 1.0f / size.z();

        GLfloat zCurrentTexture = 0.0f;

        GLfloat w = physicalSize.x() / 2.0f * scaling.x() / scalingFactor;
        GLfloat h = physicalSize.y() / 2.0f * scaling.y() / scalingFactor;

        for (int i = 0; i != (int) size.z(); ++ i) {
            vertices->push_back(ModelInfo::VertexVT(- w, - h, zCurrent, 0.0f, 1.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(- w, h, zCurrent, 0.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(w, h, zCurrent, 1.0f, 0.0f, zCurrentTexture));
            vertices->push_back(ModelInfo::VertexVT(w, - h, zCurrent, 1.0f, 1.0f, zCurrentTexture));

            indices->push_back(4 * i);
            indices->push_back(4 * i + 2);
            indices->push_back(4 * i + 1);
            indices->push_back(4 * i);
            indices->push_back(4 * i + 3);
            indices->push_back(4 * i + 2);

            zCurrent += step;
            zCurrentTexture += stepTexture;
        };

        ModelInfo::BuffersVT buffers;

        buffers.vertices = ModelInfo::VerticesVTPointer(vertices);
        buffers.indices = ModelInfo::IndicesPointer(indices);
        
        fillBuffers<ModelInfo::BuffersVT>(buffers);
    }

    Camera::ModelMatrix VolumeModel::model(const Viewport::Viewport * viewport) const {
        Camera::ModelMatrix model = viewport->modelTextureBillboard();
        Camera::Rotation axisSwap = viewport->orientationBillboard();

        model.rotate(axisSwap * orientationQuat() * INVERSE_QUAT(axisSwap));

        return model;
    }
    
    Camera::ViewMatrix VolumeModel::lightView(const Viewport::Viewport * viewport) const {
        Camera::ModelMatrix lightView = view(viewport);
        lightView.rotate(viewport->orientationBillboard());
        
        return lightView;
    }

    void VolumeModel::rotate(const QVector3D & rotation, const qreal & speed) {
        pointsModel()->rotate(rotation, speed);
        AbstractModel::rotate(rotation, speed);
    }

    Camera::Matrix VolumeModel::mvp(const Viewport::Viewport * viewport) const {
        return projection(viewport) * view(viewport) * AbstractModel::model(viewport);
    }

    Camera::Matrix VolumeModel::childsMVP(const Viewport::Viewport * viewport, const AbstractModel * child) const {
        //qDebug() << child->model(viewport);
        return projection(viewport) * view(viewport) * child->model(viewport);
    }

    void VolumeModel::drawingRoutine() const {
        glDrawElements(GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, 0);
    }

    void VolumeModel::glStatesEnable() const {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        AbstractModelWithPoints::glStatesEnable();
    }

    void VolumeModel::glStatesDisable() const {
        glDisable(GL_BLEND);

        AbstractModelWithPoints::glStatesDisable();
    }

    void VolumeModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        program->enableAttributeArray(attributeArrays["vertex"]);
        program->setAttributeBuffer(attributeArrays["vertex"], GL_FLOAT, 0, 3, stride());

        program->enableAttributeArray(attributeArrays["tex"]);
        program->setAttributeBuffer(attributeArrays["tex"], GL_FLOAT, sizeof(GLfloat) * 3, 3, stride());
    }

    void VolumeModel::bindUniformValues(QOpenGLShaderProgram * program, const Viewport::Viewport * viewport) const {
        program->setUniformValue(uniformValues["view"], view(viewport));
        program->setUniformValue(uniformValues["model"], model(viewport));
        program->setUniformValue(uniformValues["projection"], projection(viewport));
        
        program->setUniformValue(uniformValues["lightView"], lightView(viewport));

        program->setUniformValue(uniformValues["scale"], scaleMatrix());
        program->setUniformValue(uniformValues["eye"], QVector4D(viewport->eye()));

        program->setUniformValue(uniformValues["modelBillboard"], viewport->modelBillboard());

        program->setUniformValue(uniformValues["slope"], _slope);
        program->setUniformValue(uniformValues["intercept"], _intercept);

        program->setUniformValue(uniformValues["windowCenter"], _windowCenter);
        program->setUniformValue(uniformValues["windowWidth"], _windowWidth);

        program->setUniformValue(uniformValues["huRange"], _huRange);
        program->setUniformValue(uniformValues["valueRange"], _valueRange);
    }

    VolumeInfo::Slope VolumeModel::slope() const {
        return _slope;
    }

    VolumeInfo::Intercept VolumeModel::intercept() const {
        return _intercept;
    }

    VolumeInfo::WindowCenter VolumeModel::windowCenter() const {
        return _windowCenter;
    }

    VolumeInfo::WindowWidth VolumeModel::windowWidth() const {
        return _windowWidth;
    }

    VolumeInfo::HuRange VolumeModel::huRange() const {
        return _huRange;
    }

    VolumeInfo::ValueRange VolumeModel::valueRange() const {
        return _valueRange;
    }

    void VolumeModel::setSlope(const VolumeInfo::Slope & slope) {
        _slope = slope;
    }

    void VolumeModel::setIntercept(const VolumeInfo::Intercept & intercept) {
        _intercept = intercept;
    }

    void VolumeModel::setWindowCenter(const VolumeInfo::WindowCenter & windowCenter) {
        _windowCenter = windowCenter;
    }

    void VolumeModel::setWindowWidth(const VolumeInfo::WindowWidth & windowWidth) {
        _windowWidth = windowWidth;
    }

    void VolumeModel::setHuRange(const VolumeInfo::HuRange & huRange) {
        _huRange = huRange;
    }

    void VolumeModel::setValueRange(const VolumeInfo::ValueRange & valueRange) {
        _valueRange = valueRange;
    }
}

REGISTER_TYPE(VolumeModel)
