﻿#ifndef GLMANAGER_H
#define GLMANAGER_H

class GLManager
{
public:
    GLManager();
    ~GLManager();

    bool initializeGLFunctionPointers();
    void readShaderFile(const char *vertexPath, const char *fragmentPath);
    void setVertexArray(float *vertices, int vertSize, int location, int vertStep);
    void paintTriangles(int firstIndex, int count);

private:
    bool                m_isInitializeSuccess;
    unsigned int        m_programID;
    unsigned int        m_vbo; // 顶点缓冲对象
    unsigned int        m_vao; // 顶点数组对象
    char                m_errorLog[512];

};


#endif // GLMANAGER_H
