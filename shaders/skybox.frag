#version 330 core

in vec3 texCoord;

out vec4 fragColour;

uniform samplerCube skybox;

void main() {
	fragColour = texture(skybox, texCoord);
}