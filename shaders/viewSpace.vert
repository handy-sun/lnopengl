#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColors;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aNormal;

out vec2 texCoord;
out vec3 objColor;
out vec3 objNormal;
out vec3 FragPos;
out vec3 lightingPos;

uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    objColor = aColors;
    texCoord = aTexCoords;
    objNormal = mat3(transpose(inverse(view * model))) * aNormal;
    lightingPos = vec3(view * vec4(lightPos, 1.0));
}
