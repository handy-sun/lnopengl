﻿#include "shaderprogram.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef GLMANAGER_USE_GLEW
#   include "glew/glew.h"
#else
#   include "glad/glad.h"
#endif

#include "common.h"
#include "assimp/material.h"

ShaderProgram::ShaderProgram()
    : m_programID(0)
{
}

ShaderProgram::~ShaderProgram()
{
#ifdef WIN32
    //glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_programID);
#endif
}

void ShaderProgram::use()
{
    glUseProgram(m_programID);
}

void ShaderProgram::setInt1(const char *varName, int index)
{
    glUniform1i(glGetUniformLocation(m_programID, varName), index);
}

void ShaderProgram::setFloat(const char *varName, float val)
{
    glUniform1f(glGetUniformLocation(m_programID, varName), val);
}

void ShaderProgram::setVec3(const char *varName, const float *ptr)
{
    glUniform3fv(glGetUniformLocation(m_programID, varName), 1, ptr);
}

void ShaderProgram::setVec3(const char *varName, float v0, float v1, float v2)
{
    glUniform3f(glGetUniformLocation(m_programID, varName), v0, v1, v2);
}

void ShaderProgram::setMat4(const char *varName, const float *ptr)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programID, varName), 1, GL_FALSE, ptr);
}

void ShaderProgram::addShaderSourceCode(const char *code, unsigned int type)
{
    unsigned int shader;
    int success;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, m_errorLog);
        switch (type) {
        case GL_VERTEX_SHADER:
            std::cerr << "Vertex Shader Compiles Failed: " << m_errorLog << std::endl;
            break;
        case GL_FRAGMENT_SHADER:
            std::cerr << "Fragment Shader Compiles Failed: " << m_errorLog << std::endl;
            break;
        case GL_GEOMETRY_SHADER:
            std::cerr << "Geometry Shader Compiles Failed: " << m_errorLog << std::endl;
            break;        
        default:
            std::cerr << "None Error" << m_errorLog << std::endl;
            break;
        }
    } else {
        m_vecShaderId.push_back(shader);
    }
}

void ShaderProgram::addShaderFile(const char *path, unsigned int type)
{   
    std::string code;
    std::ifstream inFile;
    std::stringstream strStream;

    inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        inFile.open(path);
        strStream << inFile.rdbuf();
        inFile.close();
        code = strStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
    }
    addShaderSourceCode(code.c_str(), type);
}

void ShaderProgram::linkShaderProgram()
{
    m_programID = glCreateProgram();
    int success;
    for (unsigned int shaderId : m_vecShaderId) {
        glAttachShader(m_programID, shaderId);
    }
    glLinkProgram(m_programID);
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programID, 512, NULL, m_errorLog);
        std::cerr << "Shader Program Link Failed: " << m_errorLog << std::endl;
    }
}

void ShaderProgram::addShaderSourceCode(const char *vShaderCode, const char *fShaderCode)
{
    if (m_programID != 0)
        return;

    unsigned int vertexShader, fragmentShader;
    int success;

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, m_errorLog);
        std::cerr << "Vertex Shader Compiles Failed: " << m_errorLog << std::endl;
    };

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, m_errorLog);
        std::cerr << "Fragment Shader Compiles Failed: " << m_errorLog << std::endl;
    }

    // Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programID, 512, NULL, m_errorLog);
        std::cerr << "Shader Program Link Failed: " << m_errorLog << std::endl;
    }

    // Delete Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::addShaderFile(const char *vertexPath, const char *fragmentPath)
{
    if (m_programID != 0)
        return;
        
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    std::stringstream vShaderStream, fShaderStream;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
    }

    addShaderSourceCode(vertexCode.c_str(), fragmentCode.c_str());
}

void ShaderProgram::addGeometryShaderSourceCode(const char *geoCode)
{
    if (m_programID == 0)
        return;

    unsigned int geoShader;
    int success;

    geoShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geoShader, 1, &geoCode, NULL);
    glCompileShader(geoShader);
    glGetShaderiv(geoShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geoShader, 512, NULL, m_errorLog);
        std::cerr << "Geometry Shader Compiles Failed: " << m_errorLog << std::endl;
    }
    glAttachShader(m_programID, geoShader);
    glLinkProgram(m_programID);
}

void ShaderProgram::addGeometryShaderFile(const char *geoPath)
{
    if (m_programID == 0)
        return;
        
    std::string geoCode;
    std::ifstream geoFile;
    std::stringstream geoStream;

    geoFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        geoFile.open(geoPath);
        geoStream << geoFile.rdbuf();
        geoFile.close();
        geoCode = geoStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cerr << "ERROR::GEOMETRY_SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
    }

    addGeometryShaderSourceCode(geoCode.c_str());
}

unsigned int ShaderProgram::genVertexArray()
{
    uint VAO;
    glGenVertexArrays(1, &VAO);
    m_vecVaos.push_back(VAO);

    return VAO;
}

void ShaderProgram::setVertexAttribute(int location, int vertStep, const float *vertices, int vertSize, unsigned int VAO)
{
    if (m_vecVaos.empty()) {
        genVertexArray();
    }

    if (VAO == 0) {
        glBindVertexArray(*(m_vecVaos.cend() - 1));
    } else {
        glBindVertexArray(VAO);
    }

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, vertStep, GL_FLOAT, GL_FALSE, vertStep * sizeof(float), (void*)0);

    // release bind: VBO, VAO
    // 由于 glVertexAttribPointer 已经将 具体 VBO数据 绑定到 对应 VAO 上了, 所以，现在 解绑 VBO 是允许的
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //    glBindVertexArray(0); ???
}

void ShaderProgram::setVertexMultiAttribute(void *data, int vertSize, unsigned int VAO)
{
    if (m_vecVaos.empty()) {
        genVertexArray();
    }

    if (VAO == 0) {
        glBindVertexArray(*(m_vecVaos.cend() - 1));
    } else {
        glBindVertexArray(VAO);
    }

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
}

unsigned int ShaderProgram::genFrameBuffer()
{
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    return FBO;
}

unsigned int ShaderProgram::genRenderBuffer(int width, int height)
{
    unsigned int textureColorbuffer, RBO;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    m_vecTexturesId.push_back(textureColorbuffer);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return RBO;
}

unsigned int ShaderProgram::setIndexArray(const unsigned int *idx, int size)
{
    uint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idx, GL_STATIC_DRAW);
    m_vecEboSize.push_back(size / sizeof(unsigned int));
    return EBO;
}

unsigned int ShaderProgram::genImageData(unsigned char *imageData, int width, int height, int channel)
{
    uint textureId, format;

    if (channel == 1)
        format = GL_RED;
    else if (channel == 3)
        format = GL_RGB;
    else if (channel == 4)
        format = GL_RGBA;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);  

    m_vecTexturesId.push_back(textureId);
    
    return textureId;
}

void ShaderProgram::drawElementWithOneTex(int indicesCount, int index)
{
    if (index >= m_vecTexturesId.size()) {
        std::cout << "wrong = " << m_vecTexturesId.size() << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_vecTexturesId.at(index));
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void *)0);
}

void ShaderProgram::drawTrianglesElements(int steps, int onceCount)
{    
    glUseProgram(m_programID);

    for (uint _vao : m_vecVaos) {
        glBindVertexArray(_vao);
        for (int i = 0; i < m_vecTexturesId.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_vecTexturesId.at(i));
        }

        for (int i = 0; i < steps; ++i) {
            glDrawElements(GL_TRIANGLES, onceCount, GL_UNSIGNED_INT, reinterpret_cast<void *>(onceCount * i * sizeof(GLuint)));
        }
        glBindVertexArray(0);
    }
}

void ShaderProgram::drawTrianglesArrays(int steps, int onceCount)
{
    glUseProgram(m_programID);

    for (uint _vao : m_vecVaos) {
        glBindVertexArray(_vao);
        for (int i = 0; i < m_vecTexturesId.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_vecTexturesId.at(i));
        }

        for (int i = 0; i < steps; ++i) {
            glDrawArrays(GL_TRIANGLES, 0, onceCount);
        }
        glBindVertexArray(0);
    }
}

void ShaderProgram::drawModels()
{
    glUseProgram(m_programID);
    Texture _tex;
        
    for (int index = 0; index < m_vecVaos.size(); ++index) {
        glBindVertexArray(m_vecVaos.at(index));

        int number;
        int counts[4] = { 1, 1, 1, 1 };
        for (int i = 0; i < m_vecTex.at(index).size(); ++i) {
            _tex = m_vecTex.at(index).at(i);
            std::string frName;
            switch (_tex.type) {
            case aiTextureType_DIFFUSE:
                frName = "texture_diffuse" + std::to_string(counts[0]);
                ++counts[0];
                break;
            case aiTextureType_SPECULAR:
                frName = "texture_specular" + std::to_string(counts[1]);
                ++counts[1];
                break;
            case aiTextureType_NORMALS:
                frName = "texture_normal" + std::to_string(counts[2]);
                ++counts[2];
                break;
            case aiTextureType_AMBIENT:
                frName = "texture_height" + std::to_string(counts[3]);
                ++counts[3];
                break;            
            default:
                break;
            }

            glActiveTexture(GL_TEXTURE0 + i);
            if (!frName.empty())
                glUniform1i(glGetUniformLocation(m_programID, frName.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, _tex.id);
        }
        glDrawElements(GL_TRIANGLES, m_vecEboSize.at(index), GL_UNSIGNED_INT, (void *)0);
        glBindVertexArray(0);
    }
}
