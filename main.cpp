#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define GLAD_GL_IMPLEMENTATION

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <math.h>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "ShaderModel.h"
#include "QuadRender.h"
#include "RenderFramebuffer.h"


#include <iostream>
#include <random>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void Init();
void DeInit();

typedef unsigned int uint;

const unsigned int TEX_WIDTH = 1024;
const unsigned int TEX_HEIGHT = 1024;

const unsigned int CROP_TEX_WIDTH = 224;
const unsigned int CROP_TEX_HEIGHT = 224;

unsigned int window_width = 800;
unsigned int window_height = 800;

GLFWwindow* window = nullptr;
RenderFramebuffer* textureFramebuffer = nullptr;
QuadRender* screenQuad = nullptr;
Shader *screenShader = nullptr;

void PrepareRenderToTexture();
void RenderTextureToScreen();
void SaveCroppedTexturesToFile( int shader_index );
void ResetBinds();

int output_file_index = 0;

std::vector<ShaderModel*> Shaders;
std::vector<Material*> Materials;

class RenderSetting
    {
    public:
        unsigned int MaterialID;
        glm::mat4 Transform;
        glm::vec2 BlendCenter;
        float BlendInnerRadius;
        float BlendOuterRadius;
    };

class ImageCrop
    {
    public:
        int x, y;
    };

std::vector<RenderSetting> RenderSettings;
std::vector<ImageCrop> ImageCrops;

// rand 0->1
float Random()
    {
    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::uniform_real_distribution<> dist(0, 1);

    return (float)dist(e2);
    }

float RandomRange( float minv , float maxv )
    {
    float a = Random();
    return minv + ((maxv - minv) * a);
    }

void StartRun()
    {
    // setup random render settings
    int renders = (rand() % 10) + 3;
    for (int r = 0; r < renders; ++r)
        {
        RenderSettings.push_back(RenderSetting());

        RenderSettings.back().MaterialID = rand() % Materials.size();

        glm::mat4 transform = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
        transform = glm::rotate(transform, glm::radians(Random()*3600.f), glm::vec3(0, 0, 1));
        transform = glm::translate(transform, glm::vec3(-0.5f, -0.5f, 0.0f));
        transform = glm::translate(transform, glm::vec3(RandomRange(-0.5f,0.5f), RandomRange(-0.5f, 0.5f), 0.0f));
        RenderSettings.back().Transform = transform;

        glm::vec2 origin = glm::vec2(0.5f, 0.5f);
        glm::vec2 delta = glm::vec2(RandomRange(-3.f, 3.f), RandomRange(-3.f, 3.f));
        glm::vec2 cent = origin + delta;
        float dist_from_origin = length(delta);

        float overlap = RandomRange(-0.3f, 0.3f);
        float masklength = RandomRange(0.01f, 0.1f);

        // if this is the first layer, make sure it is totally covering
        if (r == 0)
            overlap = 2.f;

        RenderSettings.back().BlendCenter = cent;
        RenderSettings.back().BlendInnerRadius = dist_from_origin + overlap;
        RenderSettings.back().BlendOuterRadius = dist_from_origin + overlap + masklength;
        }

    // setup random crops of the images
    int crops = (rand() % 10) + 5;
    for (int c = 0; c < crops; ++c)
        {
        ImageCrop i = { 0,0 };
        int rx = TEX_WIDTH - CROP_TEX_WIDTH;
        if (rx > 0) { i.x = (int)(round(RandomRange(0.f, float(rx)))); }
        int ry = TEX_HEIGHT - CROP_TEX_HEIGHT;
        if (ry > 0) { i.y = (int)(round(RandomRange(0.f, float(ry)))); }
        ImageCrops.push_back(i);
        }
    }

void EndRun()
    {
    RenderSettings.clear();
    ImageCrops.clear();
    }

void MakeSureDirectoryExists(const char *Path)
    {
    DWORD dwAttrib = GetFileAttributes(Path);
    bool exists = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    if (!exists)
        {
        CreateDirectory(Path,nullptr);
        }
    }

int main()
    {
    Init();

    MakeSureDirectoryExists("output\\images");
    MakeSureDirectoryExists("output\\labels");

    Shaders.push_back(ShaderModel::New("Shaded"));
    Shaders.push_back(ShaderModel::New("BaseColor"));
    Shaders.push_back(ShaderModel::New("NormalsAO"));
    //Shaders.push_back(ShaderModel::New("Normals"));
    //Shaders.push_back(ShaderModel::New("AO"));

    // ###############################################################

    // allocate materials
    Materials.push_back(Material::New("Coffee_Grains_001"));
    Materials.push_back(Material::New("Metal_Tiles_002"));
    Materials.push_back(Material::New("Rocks_Hexagons_002"));
    Materials.push_back(Material::New("Bark_06"));
    Materials.push_back(Material::New("Grass_005"));
    Materials.push_back(Material::New("Leather_005"));
    Materials.push_back(Material::New("Leather_008"));
    Materials.push_back(Material::New("Leather_Armor_002"));
    Materials.push_back(Material::New("Wood_Crate_001"));
    Materials.push_back(Material::New("Wood_Herringbone_Tiles_003"));
    Materials.push_back(Material::New("Wood_Wall_003"));
    Materials.push_back(Material::New("Wood_Wicker_006"));

    output_file_index = 0;

    // render loop
    while (!glfwWindowShouldClose(window))
        {
        PrepareRenderToTexture();
        
        StartRun();

        // for each shader
        for (size_t shader_index = 0; shader_index < Shaders.size(); ++shader_index)
            {
            // render container
            Shader* shader = Shaders[shader_index]->Shad;

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color to 
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

            // for each render setting
            for (size_t render_setting_index = 0; render_setting_index < RenderSettings.size(); ++render_setting_index)
                {
                RenderSetting render_settings = RenderSettings[render_setting_index];

                // bind textures on corresponding texture units
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Materials[render_settings.MaterialID]->BaseColor->ID);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, Materials[render_settings.MaterialID]->Normals->ID);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, Materials[render_settings.MaterialID]->AO->ID);

                // setup shader settings
                shader->use();
                shader->setInt("ColorTexture", 0);
                shader->setInt("NormalTexture", 1);
                shader->setInt("AOTexture", 2);
                shader->setVec2("BlendCenter",render_settings.BlendCenter);
                shader->setFloat("BlendInnerRadius", render_settings.BlendInnerRadius);
                shader->setFloat("BlendOuterRadius", render_settings.BlendOuterRadius);
                shader->setMat4("Transform", render_settings.Transform);

                screenQuad->Render();
                }

            SaveCroppedTexturesToFile((int)shader_index);
            }

        EndRun();

        output_file_index += 1;

        // switch to rendering the quad on screen
        RenderTextureToScreen();
               
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
            glfwSetWindowShouldClose(window, true);
            }
        }

    // remove materials
    for (size_t i = 0; i < Materials.size(); ++i)
        {
        delete Materials[i];
        }
    Materials.clear();


    DeInit();

    return 0;
    }

void ResetBinds()
    {
    // for good measure, reset and unbind stuff
    for (uint i = 0; i < 16; ++i)
        {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
        }
    glUseProgram(0);
    }

void PrepareRenderToTexture()
    {
    ResetBinds();
    
    // prepare render to texture as framebuffer destinations
    textureFramebuffer->BindAsDestination();
    
    // render-specific settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

void SaveCroppedTexturesToFile( int shader_index )
    {
    ResetBinds();

    // get full image
    char* outBuffer = (char*)malloc(TEX_WIDTH * TEX_HEIGHT * 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureFramebuffer->ColorTexturebufferID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, outBuffer);

    stbi_flip_vertically_on_write(true);

    char* cropBuffer = (char*)malloc(CROP_TEX_WIDTH * CROP_TEX_HEIGHT * 3);

    for (size_t c = 0; c < ImageCrops.size(); ++c)
        {
        std::string out_name;
        char s[25];
        sprintf_s(s, "%04d_%02d", (int)(output_file_index) , int(c));
        if (shader_index == 0)
            {
            out_name = std::string("output\\images\\") + s + ".png";
            }
        else
            {
            out_name = std::string("output\\labels\\") + s + "_" + Shaders[shader_index]->Name + ".png";
            }

        // crop out image part
        ImageCrop i = ImageCrops[c];
        for (uint y = 0; y < CROP_TEX_HEIGHT; ++y)
            {
            for (uint x = 0; x < CROP_TEX_WIDTH; ++x)
                {
                for( uint q=0; q<3; ++q)
                    {
                    cropBuffer[((y * CROP_TEX_WIDTH) + x) * 3 + q] = outBuffer[(((y+i.y) * TEX_WIDTH) + (x+i.x)) * 3 + q];
                    }
                }
            }

        // store texture to file
        stbi_write_png(out_name.c_str(), CROP_TEX_WIDTH, CROP_TEX_HEIGHT, 3, cropBuffer, 0);
        }

    free(cropBuffer);
    free(outBuffer);
    }

void RenderTextureToScreen()
    {
    ResetBinds();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);
    glDisable(GL_BLEND);

    // use screen shader and reset transforms to 0->1
    screenShader->use();
    glm::mat4 transform = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
    screenShader->setMat4("Transform", transform);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureFramebuffer->ColorTexturebufferID);

    screenQuad->Render();
    }

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void Init()
    {
// glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(window_width, window_height, "", NULL, NULL);
    if (window == NULL)
        {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
        }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL(glfwGetProcAddress))
        {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
        }

    // set up global stuff
    textureFramebuffer = RenderFramebuffer::Create(TEX_WIDTH, TEX_HEIGHT);
    screenQuad = QuadRender::Create();
    
    screenShader = Shader::Load("shaders/toScreen.vert", "shaders/toScreen.frag");
    screenShader->use();
    screenShader->setInt("ColorTexture", 0);

    ResetBinds();
    }

void DeInit()
    {
    ResetBinds();

    for (size_t i = 0; i < Shaders.size(); ++i)
        {
        delete Shaders[i];
        }

    delete screenShader;
    delete screenQuad;
    delete textureFramebuffer;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    }

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
    window_width = window_height = (width < height) ? width : height;
    }
