#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];

out highp float depth;

out vec4 normal;

void main()
{
	depth = (viewMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0)).z;
	gl_Position = projectionMatrix * viewMatrix * objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
	normal = vec4(normalize(mat3(transpose(inverse(viewMatrix * objPosMatrix[gl_InstanceID]))) * vNormal), 1.0);
}