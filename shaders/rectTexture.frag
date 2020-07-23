#version 330 core

out vec4 FragColor;
in vec2 outTex;

uniform sampler2D sampTex1;
uniform sampler2D sampTex2;

void main()
{
    FragColor = mix(texture(sampTex1, outTex), texture(sampTex2, outTex), 0.3);
}
