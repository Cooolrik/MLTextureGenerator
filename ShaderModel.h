#pragma once

#include "Shader.h"
#include <string>

class ShaderModel
    {
    private:
        ShaderModel() {}

    public:
        std::string Name{};
        Shader* Shad = nullptr;

        static ShaderModel* New(std::string name);

        ~ShaderModel();
    };