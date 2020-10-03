#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <vector>
#include "common.h"

struct Vertex;
struct Texture;

class ShaderProgram
{
    typedef unsigned int uint;
    typedef std::vector<uint> VectorUint;
    typedef std::vector<Texture> VectorTex;
    typedef std::vector<VectorTex> Vec2Tex;
public:
    ShaderProgram();
    ~ShaderProgram();

    inline unsigned int programId() const { return m_programID; }
    inline void pushTexMessage(const VectorTex &_tex) { m_vecTex.push_back(_tex); }

    void use();
    void setInt1(const char *varName, int index);
    void setFloat(const char *varName, float val);
    void setVec3(const char *varName, const float *ptr);
    void setVec3(const char *varName, float v0, float v1, float v2);
    void setMat4(const char *varName, const float *ptr);
    // 新
    void addShaderSourceCode(const char *code, unsigned int type);
    void addShaderFile(const char *path, unsigned int type);
    void linkShaderProgram();
    // 旧
    void addShaderSourceCode(const char *vShaderCode, const char *fShaderCode);
    void addShaderFile(const char *vertexPath, const char *fragmentPath);

    void addGeometryShaderSourceCode(const char *geoCode);
    void addGeometryShaderFile(const char *geoPath);

    unsigned int genVertexArray();
    void setVertexAttribute(int location, int vertStep, const float *vertices, int vertSize, unsigned int VAO = 0);
    void setVertexMultiAttribute(void *data, int vertSize, unsigned int VAO = 0);

    unsigned int genFrameBuffer();
    unsigned int genRenderBuffer(int width, int height);
    unsigned int setIndexArray(const unsigned int *idx, int size);
    unsigned int genImageData(unsigned char *imageData, int width, int height, int channel = 3);
    //unsigned int loadCubemap(unsigned char faceBits[][6], int width, int height, int channel);
    void drawElementWithOneTex(int indicesCount, int index);
    void drawTrianglesElements(int steps, int onceCount);
    void drawTrianglesArrays(int steps, int onceCount);
    void drawModels();

private:
    unsigned int        m_programID;
    VectorUint          m_vecShaderId;
    VectorUint          m_vecVaos; // 顶点数组对象
    VectorUint          m_vecEboSize;
    VectorUint          m_vecTexturesId;
    Vec2Tex             m_vecTex;

    char                m_errorLog[512]; // 收集错误信息
};


#endif // SHADERPROGRAM_H
