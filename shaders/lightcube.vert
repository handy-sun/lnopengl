#version 330 core

layout (location = 0) in vec3 aPos;
uniform mat4 modViewProj;

void main()
{
    gl_Position = modViewProj * vec4(aPos, 1.0);
}
