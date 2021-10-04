#include "Material.h"

#include <Windows.h>
#include <string>

static const char *extensions[] = { "png", "tga", "bmp", "jpg", "jpeg" };

static const char* basecolor[] = { "basecolor", "color", "albedo", nullptr };
static const char* normals[] = { "normals", "normal", "nrm", nullptr };
static const char* ao[] = { "ao", "ambientocclusion", "ambient", "amb", nullptr };

Texture* TryLoadTexture(const char* basename, const char* postfixes[])
    {
    for(int q=0; postfixes[q] != nullptr; ++q )
        {
        for (size_t i = 0; i < sizeof(extensions)/sizeof(extensions[0]); ++i)
            {
            std::string path("images/");
            path += basename;
            path += "_";
            path += postfixes[q];
            path += ".";
            path += extensions[i];

            if (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES)
                {
                Texture *p = Texture::Load(path.c_str());
                return p;
                }
            }
        }

    printf("error missing texture");
    exit(10000);

    return nullptr;
    }



Material* Material::New(const char* name)
    {
    Material* pThis = new Material();

    pThis->BaseColor = TryLoadTexture(name, basecolor);
    pThis->Normals = TryLoadTexture(name, normals);
    pThis->AO = TryLoadTexture(name, ao);

    return pThis;
    }

Material::~Material()
    {
    delete this->BaseColor;
    delete this->Normals;
    delete this->AO;
    }



