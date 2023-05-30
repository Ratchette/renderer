#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 textureCoordinates;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 perspectiveTransform;

void main() {
    textureCoordinates = aTexCoords;    
    gl_Position = perspectiveTransform * viewTransform * modelTransform * vec4(aPos, 1.0);
}