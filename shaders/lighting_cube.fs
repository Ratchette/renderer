#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPosition;
in vec3 LightPosition;

uniform Material material;
uniform Light light;

out vec4 FragColor;

void main() {
	vec3 ambient = light.ambient * material.ambient ;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPosition - FragPosition);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = material.diffuse * diff * light.diffuse;

	vec3 viewDir = normalize(-FragPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = material.specular * spec * light.ambient;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}