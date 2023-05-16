#version 330 core

in vec3 vertexColour;
in vec2 texCoord;

out vec4 FragColour;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
   FragColour = mix(texture(texture0, texCoord), texture(texture1, texCoord), 0.2);
}