#version 330 core

out vec4 FragColor;

uniform vec3 objectColour;
uniform vec3 lightColour;

void main() {
	FragColor = vec4(objectColour * lightColour, 1.0);
}