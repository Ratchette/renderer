#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 perspectiveTransform;

out vec3 Normal;
out vec3 FragPosition;

void main() {
	gl_Position = perspectiveTransform * viewTransform * modelTransform * vec4(aPos, 1.0f);
	FragPosition = vec3(modelTransform * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(modelTransform))) * aNormal;
}