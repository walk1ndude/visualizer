#include "Model/VolumeModel.h"

namespace Model {
    VolumeModel::VolumeModel(Scene::AbstractScene * scene,
                         const ShaderInfo::ShaderFiles & shaderFiles,
                         const ShaderInfo::ShaderVariablesNames & shaderAttributeArrays,
                         const ShaderInfo::ShaderVariablesNames & shaderUniformValues) :
        AbstractModelWithPoints(scene, shaderFiles, shaderAttributeArrays, shaderUniformValues) {
        lockToModelAxis();
        //lockToWorldAxis();
    }

    void VolumeModel::init(const ModelInfo::Params & params) {
        AbstractModelWithPoints::init(params);

        VolumeInfo::PhysicalSize physicalSize = params["physicalSize"].value<VolumeInfo::PhysicalSize>();
        VolumeInfo::Size size = params["size"].value<VolumeInfo::Size>();
        VolumeInfo::Scaling scaling = params["scaling"].value<VolumeInfo::Scaling>();

        scale(scaling);

        setSlope(params["slope"].value<VolumeInfo::Slope>());
        setIntercept(params["intercept"].value<VolumeInfo::Intercept>());

        setWindowCenter(params["windowCenter"].value<VolumeInfo::WindowCenter>());
        setWindowWidth(params["windowWidth"].value<VolumeInfo::WindowWidth>());

        setHuRange(params["huRange"].value<VolumeInfo::HuRange>());
        setValueRange(params["valueRange"].value<VolumeInfo::ValueRange>());

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

    void VolumeModel::glStatesEnable(const RenderState & state) const {
        switch (state) {
        case CORE_RENDER:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case CONTOUR_RENDER:
            break;
        }

        AbstractModel::glStatesEnable();
    }

    void VolumeModel::glStatesDisable(const RenderState & state) const {
        switch (state) {
        case CORE_RENDER:
            glDisable(GL_BLEND);
            break;
        case CONTOUR_RENDER:
            break;
        }

        AbstractModel::glStatesDisable();
    }

    void VolumeModel::bindAttributeArrays(QOpenGLShaderProgram * program) const {
        QMutexLocker locker (&modelMutex);

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
        QMutexLocker locker (&modelMutex);

        _slope = slope;
    }

    void VolumeModel::setIntercept(const VolumeInfo::Intercept & intercept) {
        QMutexLocker locker (&modelMutex);

        _intercept = intercept;
    }

    void VolumeModel::setWindowCenter(const VolumeInfo::WindowCenter & windowCenter) {
        QMutexLocker locker (&modelMutex);

        _windowCenter = windowCenter;
    }

    void VolumeModel::setWindowWidth(const VolumeInfo::WindowWidth & windowWidth) {
        QMutexLocker locker (&modelMutex);

        _windowWidth = windowWidth;
    }

    void VolumeModel::setHuRange(const VolumeInfo::HuRange & huRange) {
        QMutexLocker locker (&modelMutex);

        _huRange = huRange;
    }

    void VolumeModel::setValueRange(const VolumeInfo::ValueRange & valueRange) {
        QMutexLocker locker (&modelMutex);

        _valueRange = valueRange;
    }

    void VolumeModel::invoke(const QString & name, const ModelInfo::Params & params) {
        if (name == "setHuRange") {
            setHuRange(params["range"].value<VolumeInfo::HuRange>());
            return;
        }

        if (name == "setWindowCenter") {
            setWindowCenter(params["windowCenter"].value<VolumeInfo::WindowCenter>());
            return;
        }

        if (name == "setWindowWidth") {
            setWindowWidth(params["windowWidth"].value<VolumeInfo::WindowWidth>());
            return;
        }

        AbstractModelWithPoints::invoke(name, params);
    }
}

REGISTER_TYPE(VolumeModel)
