#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 objColor;
in vec3 objNormal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // vec4 texColor = mix(texture(sampTex1, texCoord), texture(sampTex2, texCoord), 0.3);
	// vec4 texColor = texture(sampTex1, texCoord);
    // if (texColor.a < 0.1f) discard;
    // FragColor = texColor * vec4(objColor, 1.0);
    vec3 ligthDiffuse = lightColor * vec3(0.5f);   
    vec3 ligthAmbient = ligthDiffuse * vec3(0.2f);
    vec3 lightSpecular = vec3(1.0f);
   
    vec3 ambient = ligthAmbient * objColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norLized = normalize(objNormal);
    float diff = max(dot(norLized, lightDir), 0.0);
    vec3 diffuse = ligthDiffuse * (diff * objColor);

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norLized);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = lightSpecular * (spec * vec3(0.5f, 0.5f, 0.5f));  

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}
