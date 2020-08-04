#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 objColor;
in vec3 objNormal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D sampDiffuse;
uniform sampler2D sampSpecular;

void main()
{
    const vec3 ligthDiffuse = vec3(0.5f);
    const vec3 ligthAmbient = vec3(0.2f);
    const vec3 lightSpecular = vec3(1.0f);
    vec3 diffuseMap = texture(sampDiffuse, texCoord).rgb;
    vec3 specularMap = texture(sampSpecular, texCoord).rgb;
    vec3 norLized = normalize(objNormal);
   
    vec3 ambient = ligthAmbient * diffuseMap;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norLized, lightDir), 0.0);
    vec3 diffuse = ligthDiffuse * diff * diffuseMap;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norLized);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);
    vec3 specular = lightSpecular * spec * specularMap;  

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}
// vec4 texColor = mix(texture(sampTex1, texCoord), texture(sampTex2, texCoord), 0.3);
// vec4 texColor = texture(sampTex1, texCoord);
// if (texColor.a < 0.1f) discard;
// FragColor = texColor * vec4(objColor, 1.0);
