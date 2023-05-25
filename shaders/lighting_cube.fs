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
	float innerCutoff;
	float outerCutoff;

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
	vec3 ambient = light.ambient * texture(material.diffuse, TextureCoordinates).rgb;
	vec3 lightDir = normalize(light.position - FragPosition);
	float theta  = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	float downscale = 0.10;
	vec3 emission = vec3(texture(material.emission, TextureCoordinates * (1.0 + downscale * 2) - downscale));

	// With cos, bigger number = smaller angle
	if(theta > light.outerCutoff){
		float distance = length(light.position - FragPosition);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
		vec3 norm = normalize(Normal);

		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = vec3(texture(material.diffuse, TextureCoordinates)) * diff * light.diffuse;

		vec3 viewDir = normalize(viewerPosition - FragPosition);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = vec3(texture(material.specular, TextureCoordinates)) * spec * light.ambient;

		// vec3 result = ambient + diffuse + specular + emission;
		vec3 result = ambient + (diffuse + specular) * attenuation * intensity;
		FragColor = vec4(result, 1.0);

	} else {
		FragColor = vec4((ambient), 1.0);
	}
}