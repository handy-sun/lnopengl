#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 objColor;
in vec3 objNormal;
in vec3 FragPos;
in vec3 lightingPos;

uniform vec3 lightColor;

void main()
{
    // vec4 texColor = mix(texture(sampTex1, texCoord), texture(sampTex2, texCoord), 0.3);
	// vec4 texColor = texture(sampTex1, texCoord);
    // if (texColor.a < 0.1f) discard;
    // FragColor = texColor * vec4(objColor, 1.0);
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightingPos - FragPos);
    vec3 norLized = normalize(objNormal);
    float diff = max(dot(norLized, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(- FragPos);
    vec3 reflectDir = reflect(-lightDir, norLized);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(result, 1.0);
}
