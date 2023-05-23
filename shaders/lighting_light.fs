#version 330 core

uniform vec3 lightColour;

out vec4 FragColor;

void main() {
	FragColor = vec4(lightColour, 1.0);
}