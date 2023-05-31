#version 330 core

uniform vec3 outlineColour;

out vec4 FragColour;

void main() {
	FragColour = vec4(outlineColour, 1.0);
}