#version 330 core

out vec4 FragColor;
in vec2 texCoord;
in vec3 objColor;

// uniform sampler2D sampTex1;
// uniform sampler2D sampTex2;
uniform vec3 lightColor;

void main()
{
    // vec4 texColor = mix(texture(sampTex1, texCoord), texture(sampTex2, texCoord), 0.3);
	// vec4 texColor = texture(sampTex1, texCoord);
    // if (texColor.a < 0.1f) discard;
    // FragColor = texColor * vec4(objColor, 1.0);
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objColor;
    FragColor = vec4(result, 1.0);
}
