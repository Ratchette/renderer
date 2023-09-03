#version 330 core   

in vec2 TexCoords;

uniform float screen_width;
uniform sampler2D screenTexture;

out vec4 FragColor;

void main(){
    FragColor = texture(screenTexture, vec2(screen_width - TexCoords.x, TexCoords.y));
    FragColor.w = 1.0;
}