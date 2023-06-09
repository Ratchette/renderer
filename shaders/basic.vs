#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 perspectiveTransform;

void main() {
   gl_Position = perspectiveTransform * viewTransform * modelTransform * vec4(aPos, 1.0f);
   texCoord = aTexCoord;
}