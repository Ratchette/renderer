#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 Normal;
in vec3 FragPosition;
in vec2 TextureCoordinates;

uniform vec3 viewerPosition;
uniform Material material;
uniform Light light;

out vec4 FragColor;

void main() {
	float distance = length(light.position - FragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));


	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextureCoordinates));

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPosition);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = vec3(texture(material.diffuse, TextureCoordinates)) * diff * light.diffuse;

	vec3 viewDir = normalize(viewerPosition - FragPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TextureCoordinates)) * spec * light.ambient;

	float downscale = 0.10;
	vec3 emission = vec3(texture(material.emission, TextureCoordinates * (1.0 + downscale * 2) - downscale));

	// vec3 result = ambient + diffuse + specular + emission;
	vec3 result = (ambient + diffuse + specular) * attenuation;
	FragColor = vec4(result, 1.0);
}