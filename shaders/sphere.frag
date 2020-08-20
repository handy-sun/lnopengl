#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 objNormal;
in vec3 FragPos;
in float fi;

uniform sampler2D diffuseSun;
uniform sampler2D diffuseEarth;
uniform int index;

void main()
{   
    if (index == 0) {	
		FragColor = texture(diffuseSun, texCoord);
	} else if (index == 1) {
		FragColor = texture(diffuseEarth, texCoord) * fi;
	} 
}
