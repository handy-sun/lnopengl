#ifndef COMMON_H
#define COMMON_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/vector3.h"
#include "assimp/vector2.h"
#include <string>

struct Vertex
{
    typedef aiVector3D Vector3D;
    typedef aiVector2D Vector2D;

    Vector3D Position;
    Vector3D Normal;
    Vector2D TexCoords;
    Vector3D Tangent;
    Vector3D Bitangent;
};

struct Texture
{
    unsigned int id;
    int type;
    std::string path;
};

#endif // COMMON_H
