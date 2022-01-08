#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
    sampler2D emission;
	float shininess;
};

struct Light {
	
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;


void main()
{
	vec3 lightDir = normalize(light.position - FragPos);
	
	vec3 norm = normalize(Normal);
	

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

	vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

	vec3 emission = vec3(0.0);
	emission = texture(material.emission, TexCoord).rgb;
	
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  

	diffuse  *= intensity;
	specular *= intensity;
	  
		// attenuation
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

	ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
	diffuse  *= attenuation;
	specular *= attenuation;
	  
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
	
}