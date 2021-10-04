
#include <glad/gl.h>

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

static void checkCompileErrors(unsigned int shader, std::string type)
    {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
        {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
            {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    else
        {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
            {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

Shader* Shader::Load(   const char* vertexPath,
                        const char* fragmentPath,
                        const char* vertexPreincludePath,
                        const char* fragmentPreincludePath)
    {
    return new Shader(vertexPath, fragmentPath, vertexPreincludePath, fragmentPreincludePath);
    }

std::string ReadFile(const char* path)
    {
    std::ifstream vShaderFile;
    std::string code;
 
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
        {
        // open files
        vShaderFile.open(path);

        std::stringstream vShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();

        // convert stream into string
        code = vShaderStream.str();
        }
    catch (std::ifstream::failure& e)
        {
        e;
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << path << std::endl;
        exit(-3000);
        }

    return code;
    }

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
Shader::Shader(const char* vertexPath,
    const char* fragmentPath,
    const char* vertexPreincludePath,
    const char* fragmentPreincludePath)
    {
    // 1. retrieve the vertex/fragment source code from filePath

    std::string vertexCode("");
    if(vertexPreincludePath != nullptr)
        vertexCode += ReadFile(vertexPreincludePath);
    vertexCode += ReadFile(vertexPath);

    std::string fragmentCode("");
    if (fragmentPreincludePath != nullptr)
        fragmentCode += ReadFile(fragmentPreincludePath);
    fragmentCode += ReadFile(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    }

// activate the shader
// ------------------------------------------------------------------------
void Shader::use()
    {
    glUseProgram(ID);
    }

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
    {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
    {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
    {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
    {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
void Shader::setVec2(const std::string& name, float x, float y) const
    {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
    {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
    {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
    {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
    {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
    {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
    {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

Shader::~Shader()
    {
    glDeleteShader(this->ID);
    }