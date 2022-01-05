#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D texture1;
uniform sampler2D texture2;


uniform vec3 objectColor;
uniform vec3 lightColor;


uniform float texCoef;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float ambientStrength = 0.05f;
    vec3 ambient = lightColor * ambientStrength;

	float specularStrength = 0.5f;
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(reflectDir, viewDir), 0.0f), 4);
	vec3 specular = spec * lightColor * specularStrength;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}