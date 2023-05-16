#version 330 core

in vec3 vertexColour;
in vec2 texCoord;

out vec4 FragColour;

uniform sampler2D ourTexture;

void main() {
   FragColour = texture(ourTexture, texCoord) * vec4(vertexColour, 1.0);
}