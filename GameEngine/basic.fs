#version 330 core
out vec4 FragColor;
in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;


uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float texCoef;

void main()
{
	FragColor = vec4(lightColor * objectColor, 1.0);
}