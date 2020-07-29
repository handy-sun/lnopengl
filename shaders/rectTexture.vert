#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColors;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoord;
out vec3 objColor;
uniform mat4 modViewProj;

void main()
{
    gl_Position = modViewProj * vec4(aPos, 1.0);
    objColor = aColors;
    texCoord = aTexCoords;
}
