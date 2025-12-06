#version 330 core
layout (location=0) in vec3 vPosition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];

out highp float depth;

void main()
{
	depth = viewMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0).z;
	gl_Position = projectionMatrix * viewMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
}