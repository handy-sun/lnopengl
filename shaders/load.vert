#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoord;
out vec3 objNormal;
out vec3 FragPos;
out float fi;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoord = aTexCoords;    
    objNormal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec4 center = model * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 v1 = normalize(center.xyz);
	vec3 v2 = normalize(vec3(center) - FragPos);
	fi = max(0, dot(v1, v2)) * 0.95 + 0.05;
}
