#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];
out vec2 TexCoord;
out vec4 normal;
out vec3 position;
void main()
{
	vec4 vVerticPositionInModelCoordinate = objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
    normal = normalize(objPosMatrix[gl_InstanceID] * (vec4(vNormal, 0.0) + vec4(vPosition, 1.0)) - vVerticPositionInModelCoordinate);
	gl_Position = projectionMatrix * viewMatrix * vVerticPositionInModelCoordinate;
	TexCoord = aTexCoord;
	position = vVerticPositionInModelCoordinate.xyz;
}