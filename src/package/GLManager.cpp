#include "GLManager.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef GLMANAGER_USE_GLEW
#include "glew/glew.h"
#else
#include "glad/glad.h"
#endif

GLManager::GLManager()
    : m_programID(0)
    , m_vao(0)
    , m_ebo(0)
    , m_arrayBufferCount(0)
{
    glGenVertexArrays(1, &m_vao);
    memset(m_texture, 0, 6 * sizeof(unsigned int));
}

GLManager::~GLManager()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_programID);
}

void GLManager::use()
{
    glUseProgram(m_programID);
}

void GLManager::setVec3(const char *varName, const float *ptr)
{
    glUniform3fv(glGetUniformLocation(m_programID, varName), 1, ptr);
}

void GLManager::setMat4(const char *varName, const float *ptr)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programID, varName), 1, GL_FALSE, ptr);
}

void GLManager::readShaderFile(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    std::stringstream vShaderStream, fShaderStream;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    unsigned int vertexShader, fragmentShader;
    int success;
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, m_errorLog);
        std::cerr << "Vertex Shader Compiles Failed: " << m_errorLog << std::endl;
    };

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, m_errorLog);
        std::cerr << "Fragment Shader Compiles Failed: " << m_errorLog << std::endl;
    }

    // Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_programID, 512, NULL, m_errorLog);
        std::cerr << "Shader Program Link Failed: " << m_errorLog << std::endl;
    }

    // Delete Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void GLManager::setVertexArray(int location, int vertStep, float *vertices, int vertSize)
{
    if (location == m_arrayBufferCount)
        ++m_arrayBufferCount;

    unsigned int VBO;
    glBindVertexArray(m_vao);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(location, vertStep, GL_FLOAT, GL_FALSE, vertStep * sizeof(float), (void*)0);
    glEnableVertexAttribArray(location);

    // release bind: VBO, VAO
    // 由于 glVertexAttribPointer 已经将 具体 VBO数据 绑定到 对应 VAO 上了, 所以，现在 解绑 VBO 是允许的
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
}

void GLManager::setIndexArray(unsigned int *idx, int size)
{
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, idx, GL_STATIC_DRAW);
}

void GLManager::genImageData(unsigned char *imageData, int width, int height, int index, int channel)
{
    if (index > 5 || index < 0)
        return;

    glGenTextures(1, &m_texture[index]);
    glBindTexture(GL_TEXTURE_2D, m_texture[index]);
    if (channel == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);    
}

void GLManager::paintTriangles(int steps, int onceCount)
{
    use();
//    glUniform1i(glGetUniformLocation(m_programID, "sampTex1"), 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, m_texture);

//    glUniform1i(glGetUniformLocation(m_programID, "sampTex2"), 1);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, m_texture1);
    for (int i = 0; i < m_arrayBufferCount; ++i)
    {
        glEnableVertexAttribArray(i);
    }
    glBindVertexArray(m_vao);

    if (m_ebo != 0)
    {
        for (int i = 0; i < steps; ++i)
        {
            if (m_texture[i] != 0)
                glBindTexture(GL_TEXTURE_2D, m_texture[i]);

            glDrawElements(GL_TRIANGLES, onceCount,
                           GL_UNSIGNED_INT, reinterpret_cast<void *>(onceCount * i * sizeof(GLuint)));
        }
    }
    else
    {
        for (int i = 0; i < steps; ++i)
        {
            if (m_texture[i] != 0)
                glBindTexture(GL_TEXTURE_2D, m_texture[i]);

            glDrawArrays(GL_TRIANGLE_FAN, i * onceCount, onceCount);
        }
    }
}
