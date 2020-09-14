#include "modelloader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include <glad/glad.h>

#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"

#include "common.h"
#include "shaderprogram.h"

using namespace std;

ModelLoader::ModelLoader(bool gamma)
    : m_gammaCorrection(gamma)
{
    
}

ModelLoader::ModelLoader(const string &path, bool gamma)
    : m_gammaCorrection(gamma)
{
    loadModel(path);
}

bool ModelLoader::loadModel(const string &path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate
                                                 | aiProcess_FlipUVs
                                                 | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return false;
    }

    m_directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    return true;
}

void ModelLoader::processNode(aiNode *node, const aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void ModelLoader::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<unsigned int> _indices;
    vector<Vertex> _vertices;
    vector<Texture> _textures, vecTemp;

    for (int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        memset(&vertex, 0, sizeof(Vertex));
#if 1
        vertex.Position = mesh->mVertices[i];
        if (mesh->mNormals)
            vertex.Normal = mesh->mNormals[i];
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = aiVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        if (mesh->mTangents)
            vertex.Tangent = mesh->mTangents[i];
        if (mesh->mBitangents)
            vertex.Bitangent = mesh->mBitangents[i];
#else
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->mNormals)
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        if (mesh->mTangents)
            vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        if (mesh->mBitangents)
            vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
#endif
        _vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; ++i) {
        for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
            _indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }
    
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    loadMaterialTextures(material, aiTextureType_DIFFUSE, &_textures);
    loadMaterialTextures(material, aiTextureType_SPECULAR, &_textures);
    loadMaterialTextures(material, aiTextureType_HEIGHT, &_textures);
    loadMaterialTextures(material, aiTextureType_AMBIENT, &_textures);

    m_shaderPro->genVertexArray();
    m_shaderPro->setVertexMultiAttribute((void *)_vertices.data(), _vertices.size() * sizeof(Vertex));
    m_shaderPro->setIndexArray(_indices.data(), _indices.size() * sizeof(unsigned int));
    m_shaderPro->pushTexMessage(_textures);
}

void ModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::vector<Texture> *vecTextures)
{
    int width, height, nrComponents;
    for (int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString aiStr;
        mat->GetTexture(type, i, &aiStr);
        bool hadLoaded = false;

        for (int j = 0; j < m_textures_loaded.size(); ++j) {
            if (std::strcmp(m_textures_loaded[j].path.data(), aiStr.C_Str()) == 0) {
                vecTextures->push_back(m_textures_loaded[j]);
                hadLoaded = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

        if (!hadLoaded) {
            Texture texture;
            string filename = m_directory + '/' + aiStr.C_Str();
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

            if (data) {
                texture.id = m_shaderPro->genImageData(data, width, height, nrComponents);
            } else {
                std::cerr << "Texture failed to load at path: " << filename << std::endl;
                texture.id = 0;
            }
            stbi_image_free(data);

            texture.type = type;
            texture.path = aiStr.C_Str();
           
            vecTextures->push_back(texture);
            m_textures_loaded.push_back(texture);
        }
    }
}
