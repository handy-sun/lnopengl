#ifndef GLMANAGER_H
#define GLMANAGER_H


class GLManager
{
public:
    GLManager();
    ~GLManager();

    inline unsigned int programId() const { return m_programID; }
    unsigned int textureId(int index) const;

    void use();
    void setInt1(const char *varName, int index);
    void setVec3(const char *varName, const float *ptr);
    void setMat4(const char *varName, const float *ptr);
    void readShaderFile(const char *vertexPath, const char *fragmentPath);
    void setVertexArray(int location, int vertStep, float *vertices, int vertSize);
    void setIndexArray(unsigned int *vertices, int size);
    void genImageData(unsigned char *imageData, int width, int height, int index, int channel = 3);
    void paintTriangles(int steps, int onceCount);

private:
    unsigned int        m_programID;
    unsigned int        m_vao; // 顶点数组对象
    unsigned int        m_ebo; // 索引缓冲对象
    unsigned int        m_textures[6];
    int                 m_arrayBufferCount;
    int                 m_textureCount;
    char                m_errorLog[512];

};


#endif // GLMANAGER_H
