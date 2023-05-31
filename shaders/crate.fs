#version 330 core

in vec2 texCoord;

uniform sampler2D texture0;

out vec4 FragColour;

void main() {
	FragColour = texture(texture0, texCoord);
}