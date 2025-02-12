#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <string>
#include <assimp/material.h>

class aiNode;
class aiMesh;
class aiScene;
class ShaderProgram;

struct aiMaterial;
struct Texture;
enum aiTextureType;

class ModelLoader
{
public:
    ModelLoader(bool gamma = false);
    ModelLoader(const std::string &path, bool gamma = false);
        
    inline void setShaderProgram(ShaderProgram *sp) { m_shaderPro = sp; }
    bool loadModel(const std::string &path);

private:
    bool                        m_gammaCorrection;
    ShaderProgram              *m_shaderPro;
    std::string                 m_directory;
    // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Texture>        m_textures_loaded;

    void processNode(aiNode *node, const aiScene *scene);

    void processMesh(aiMesh *mesh, const aiScene *scene);

    void loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::vector<Texture> *vecTextures);
};

#endif // MODELLOADER_H
