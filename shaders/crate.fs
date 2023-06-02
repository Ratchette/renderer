#version 330 core

in vec2 texCoord;

uniform sampler2D texture0;

out vec4 FragColour;

void main() {
	vec4 textureColour = texture(texture0, texCoord);
	if(textureColour.a < 0.1){
		discard;
	}

	FragColour = texture(texture0, texCoord);
}