#version 330 core

#define NUM_POINT_LIGHTS 4

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float beamWidth;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 textureCoordinates;

uniform vec3 viewerPosition;
uniform Material material;

uniform DirectionalLight directionalLight;
uniform PointLight pointLight[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

out vec4 FragColour;


vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 fragPosition, vec3 normal, vec3 viewDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 fragPosition, vec3 normal, vec3 viewDirection);

void main() {
	vec3 colour = vec3(0.0);

	vec3 normal = normalize(fragmentNormal);
	vec3 viewDirection = normalize(viewerPosition - fragmentPosition);
	
	colour += CalculateDirectionalLight(directionalLight, normal, viewDirection);

	for(int i=0; i < NUM_POINT_LIGHTS; i++){
		colour += CalculatePointLight(pointLight[i], fragmentPosition, normal, viewDirection);
	}

	colour += CalculateSpotLight(spotLight, fragmentPosition, normal, viewDirection);

	// colour += CalculateEmission();
	// Emissions?
	// 	float downscale = 0.10;
	// 	vec3 emission = vec3(texture(material.emission, TextureCoordinates * (1.0 + downscale * 2) - downscale));

	FragColour = vec4(colour, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
	vec3 lightDirection = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDirection, normal);

	float diffusionIntensity = max(dot(normal, lightDirection), 0.0);
	float specularIntensity = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

	vec3 ambient	= light.ambient  * texture(material.diffuse,  textureCoordinates).rgb;
	vec3 diffuse	= light.diffuse  * texture(material.diffuse,  textureCoordinates).rgb * diffusionIntensity;
	vec3 specular	= light.specular * texture(material.specular, textureCoordinates).rgb * specularIntensity;

	return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light, vec3 fragPosition, vec3 normal, vec3 viewDirection){
	vec3 lightDirection = normalize(light.position - fragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	float diffusionIntensity = max(dot(normal, lightDirection), 0.0);
	float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

	float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient	= light.ambient  * texture(material.diffuse,  textureCoordinates).rgb;
	vec3 diffuse	= light.diffuse  * texture(material.diffuse,  textureCoordinates).rgb * diffusionIntensity;
	vec3 specular	= light.specular * texture(material.specular, textureCoordinates).rgb * specularIntensity;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 fragPosition, vec3 normal, vec3 viewDirection){
	// Artifically crank up spotlight to make it easier to test
	float spotlightMultiplier = 2.0;

	vec3 lightDirection = normalize(light.position - fragPosition);
	float theta  = dot(lightDirection, normalize(-light.direction));

	vec3 ambient = light.ambient  * texture(material.diffuse,  textureCoordinates).rgb;

	//  With cos, bigger number = smaller angle
	if(theta > light.outerCutoff){
		vec3 reflectDirection = reflect(-lightDirection, normal);

		float diffusionIntensity = max(dot(normal, lightDirection), 0.0);
		float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);

		float epsilon = light.beamWidth - light.outerCutoff;
		float beamIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

		float distance = length(light.position - fragPosition);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		vec3 ambient	= light.ambient  * texture(material.diffuse,  textureCoordinates).rgb;
		vec3 diffuse	= light.diffuse  * texture(material.diffuse,  textureCoordinates).rgb * diffusionIntensity;
		vec3 specular	= light.specular * texture(material.specular, textureCoordinates).rgb * specularIntensity;

		return (ambient + diffuse + specular) * beamIntensity * attenuation * spotlightMultiplier;

	} else {
	 	return ambient;
	}
}