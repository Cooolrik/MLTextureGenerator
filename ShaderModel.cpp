#include "ShaderModel.h"

ShaderModel* ShaderModel::New(std::string name)
    {
    ShaderModel* pThis = new ShaderModel();
    pThis->Name = name;
    pThis->Shad = Shader::Load(
        (std::string("shaders/out") + pThis->Name + ".vert").c_str(),
        (std::string("shaders/out") + pThis->Name + ".frag").c_str(),
        "shaders/common.vert",
        "shaders/common.frag"
    );
    return pThis;
    }

ShaderModel::~ShaderModel()
    {
    delete Shad;
    }