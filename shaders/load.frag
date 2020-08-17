#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 objColor;
in vec3 objNormal;
in vec3 FragPos;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

const int NR_POINT_LIGHTS = 4;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
// uniform Material material;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;
const float c_shininess = 128.0f;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 material_diffamb, vec3 material_spec)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), c_shininess);
    // combine results
    vec3 ambient = light.ambient * material_diffamb;
    vec3 diffuse = light.diffuse * diff * material_diffamb;
    vec3 specular = light.specular * spec * material_spec;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffamb, vec3 material_spec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), c_shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * material_diffamb;
    vec3 diffuse = light.diffuse * diff * material_diffamb;
    vec3 specular = light.specular * spec * material_spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 material_diffamb, vec3 material_spec)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), c_shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * material_diffamb;
    vec3 diffuse = light.diffuse * diff * material_diffamb;
    vec3 specular = light.specular * spec * material_spec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{   
    vec3 norm = normalize(objNormal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 _diffAmb = vec3(texture(texture_diffuse1, texCoord));
    vec3 _spec = vec3(texture(texture_specular1, texCoord));

    vec3 result = CalcDirLight(dirLight, norm, viewDir, _diffAmb, _spec);

    for (int i = 0; i < 2; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, _diffAmb, _spec);    

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir, _diffAmb, _spec);    
    
    FragColor = vec4(result, 1.0);
}
