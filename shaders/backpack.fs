#version 330 core

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fragmentNormal;
in vec3 fragmentPosition;
in vec2 textureCoordinates;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform float specular_shininess;

uniform DirectionalLight directionalLight;
uniform vec3 viewerPosition;


out vec4 FragColour;


vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);


void main() {
    vec3 colour = vec3(0.0);

    vec3 normal = normalize(fragmentNormal);
    vec3 viewDirection = normalize(viewerPosition - fragmentPosition);
    
    colour += CalculateDirectionalLight(directionalLight, normal, viewDirection);

    FragColour = vec4(colour, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDirection, normal);

    float diffusionIntensity = max(dot(normal, lightDirection), 0.0);
    float specularIntensity = pow(max(dot(viewDirection, reflectDir), 0.0), specular_shininess);

    vec3 ambient    = light.ambient  * texture(texture_diffuse0,  textureCoordinates).rgb;
    vec3 diffuse    = light.diffuse  * texture(texture_diffuse0,  textureCoordinates).rgb * diffusionIntensity;
    vec3 specular   = light.specular * texture(texture_specular0, textureCoordinates).rgb * specularIntensity;

    return ambient + diffuse + specular;
}