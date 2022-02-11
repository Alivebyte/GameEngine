#version 330 core
struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
    sampler2D emission;
	float shininess;
};




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
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutOff;
    float outerCutOff;
};  

#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir); 
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform float alpha;
//uniform Light light;


void main()
{
	// properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec4 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
       result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    //if(result.a < 0.1) discard;

    FragColor = result;
};



vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec4 ambient  = vec4(light.ambient, alpha)  * texture(material.texture_diffuse1, TexCoord);
    vec4 diffuse  = vec4(light.diffuse, alpha)  * diff * texture(material.texture_diffuse1, TexCoord);
    vec4 specular = vec4(light.specular, alpha) * spec * texture(material.texture_specular1, TexCoord);


    return (ambient + diffuse + specular);
};  

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec4 ambient  = vec4(light.ambient, alpha)  * texture(material.texture_diffuse1, TexCoord);
    vec4 diffuse  = vec4(light.diffuse, alpha)  * diff * texture(material.texture_diffuse1, TexCoord);
    vec4 specular = vec4(light.specular, alpha) * spec * texture(material.texture_specular1, TexCoord);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //if(ambient.a < 0.1f)
    //discard;

    return (ambient + diffuse + specular);
};

vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  
    
    

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec4 ambient  = vec4(light.ambient, alpha)  * texture(material.texture_diffuse1, TexCoord);
    vec4 diffuse  = vec4(light.diffuse, alpha)  * diff * texture(material.texture_diffuse1, TexCoord);
    vec4 specular = vec4(light.specular, alpha) * spec * texture(material.texture_specular1, TexCoord);

    diffuse  *= intensity;
    specular *= intensity;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}