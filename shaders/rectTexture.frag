#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 objColor;
in vec3 objNormal;
in vec3 FragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform sampler2D sampDiffuse;
uniform sampler2D sampSpecular;

void main()
{
    const vec3 ligthDiffuse = vec3(0.8f);
    const vec3 ligthAmbient = vec3(0.1f);
    const vec3 lightSpecular = vec3(1.0f);
    const float constant = 1.0f;
    const float linear = 0.09f;
    const float quadratic = 0.032f;
    const float cutOff = cos(radians(12.5f));
    const float outerCutOff = cos(radians(17.5f));

    vec3 lightDir = normalize(lightPos - FragPos);
    float theta = dot(lightDir, normalize(-lightDirection));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);  

    float dis = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * dis + quadratic * (dis * dis));
    
    vec3 diffuseMap = texture(sampDiffuse, texCoord).rgb;
    vec3 specularMap = texture(sampSpecular, texCoord).rgb;
    vec3 norLized = normalize(objNormal);

    vec3 ambient = ligthAmbient * diffuseMap;
    
    //vec3 lightDir = normalize(-vec3(-0.2f, -1.0f, -0.3f));
    float diff = max(dot(norLized, lightDir), 0.0);
    vec3 diffuse = ligthDiffuse * diff * diffuseMap;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norLized);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = lightSpecular * spec * specularMap;  

    ambient *= attenuation;  
    diffuse *= (attenuation * intensity);
    specular *= (attenuation * intensity);   

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
// vec4 texColor = mix(texture(sampTex1, texCoord), texture(sampTex2, texCoord), 0.3);
// vec4 texColor = texture(sampTex1, texCoord);
// if (texColor.a < 0.1f) discard;
// FragColor = texColor * vec4(objColor, 1.0);
