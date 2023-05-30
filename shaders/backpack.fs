#version 330 core
out vec4 FragColour;

in vec2 textureCoordinates;

uniform sampler2D texture_diffuse0;

void main() {    
    FragColour = texture(texture_diffuse0, textureCoordinates);
}