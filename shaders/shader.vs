#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform vec4 greenColour;

out vec4 vertexColour;

void main() {
   vertexColour = greenColour;
   gl_Position = vec4(aPos, 1.0);
}