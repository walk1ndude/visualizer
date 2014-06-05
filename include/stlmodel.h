#ifndef STLMODEL_H
#define STLMODEL_H

#include "abstractmodel.h"

class StlModel : public AbstractModel {

public:
    StlModel();
    ~StlModel();

    void initModel(QOpenGLShaderProgram * program, ModelInfo::ModelBuffers & modelBuffers);

private:
    int _shaderVertex;
    int _shaderNormal;


};

#endif // STLMODEL_H
