#include "GLManager.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef GLMANAGER_USE_GLEW
#include "glew/glew.h"
#else
#include "glad/glad.h"
#endif

GLManager::GLManager()
    : m_isInitializeSuccess(true)
{

}

GLManager::~GLManager()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteProgram(m_programID);
}

bool GLManager::initializeGLFunctionPointers()
{
#ifdef GLMANAGER_USE_GLEW
    glewExperimental = GL_TRUE;
    return m_isInitializeSuccess = glewInit() == GLEW_OK;
#else
//    return m_isInitializeSuccess == gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    return true;
#endif
}

void GLManager::readShaderFile(const char *vertexPath, const char *fragmentPath)
{
    if (!m_isInitializeSuccess)
        return;

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

void GLManager::setVertexArray(float *vertices, int vertSize, int location, int vertStep)
{
    if (!m_isInitializeSuccess)
        return;

    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

    /* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s). */
    glBindVertexArray(m_vao);
    glVertexAttribPointer(location, vertStep, GL_FLOAT, GL_FALSE, vertStep * sizeof(float), (void*)0);
    glEnableVertexAttribArray(location);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void GLManager::paintTriangles(int firstIndex, int count)
{
    if (!m_isInitializeSuccess)
        return;

    glUseProgram(m_programID);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, firstIndex, count);
}
