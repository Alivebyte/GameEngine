#version 330 core
struct Material {
	sampler2D diffuse;
	sampler2D specular;
    sampler2D emission;
	float shininess;
};

struct Light {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform Material material;
uniform Light light;


void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;

	
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(reflectDir, viewDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

	vec3 emission = vec3(0.0);
	emission = texture(material.emission, TexCoord).rgb;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}