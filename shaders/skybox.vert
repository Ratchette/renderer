#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 perspectiveTransform;
uniform mat4 viewTransform;

out vec3 texCoord;

void main(){
	texCoord = aPos;
	vec4 pos = perspectiveTransform * viewTransform * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}