#version 330 core

in vec3 Normal;
in vec3 FragPosition;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;

out vec4 FragColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColour;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPosition - FragPosition);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColour;

	float specularStrength = 0.5;
	int shinyness = 32;
	vec3 viewDir = normalize(viewerPosition - FragPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shinyness);
	vec3 specular = specularStrength * spec * lightColour;

	vec3 result = (ambient + diffuse + specular) * objectColour;
	FragColor = vec4(result, 1.0);
}