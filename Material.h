#pragma once

#include "Texture.h"

class Material
    {
    private:
        Material() {}

    public:
        Texture* BaseColor = nullptr;
        Texture* Normals = nullptr;
        Texture* AO = nullptr;

        static Material* New(const char* name);

        ~Material();
    };
