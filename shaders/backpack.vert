#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 perspectiveTransform;

out vec3 fragmentNormal;
out vec3 fragmentPosition;
out vec2 textureCoordinates;

void main() {
	gl_Position = perspectiveTransform * viewTransform * modelTransform * vec4(aPos, 1.0f);
	fragmentNormal = mat3(transpose(inverse(modelTransform))) * aNormal;
	fragmentPosition = vec3(modelTransform * vec4(aPos, 1.0));

	textureCoordinates = aTexCoord;
}