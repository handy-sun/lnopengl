#ifndef GLMANAGER_H
#define GLMANAGER_H


class GLManager
{
public:
    GLManager();
    ~GLManager();

    unsigned int programId() const { return m_programID; }

    void readShaderFile(const char *vertexPath, const char *fragmentPath);
    void setVertexArray(int location, int vertStep, float *vertices, int vertSize);
    void setIndexArray(unsigned int *vertices, int size);
    void genImageData(unsigned char *imageData, int width, int height, int index, int channel = 3);
    void paintTriangles(int firstIndex, int count);

private:
    unsigned int        m_programID;
    unsigned int        m_vao; // 顶点数组对象
    unsigned int        m_ebo; // 索引缓冲对象
    unsigned int        m_texture[6];
    int                 m_arrayBufferCount;
    char                m_errorLog[512];

};


#endif // GLMANAGER_H
