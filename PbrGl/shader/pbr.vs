#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
layout (location=3) in vec3 vTangent;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 objPosMatrix[100];
uniform vec3 cameraPosition;

out vec2 TexCoord;
out vec4 normal;
out vec3 position;
out mat3 TBN;
out vec3 cameraPos;

void main()
{
	vec4 vVerticPositionInModelCoordinate = objPosMatrix[gl_InstanceID] * vec4(vPosition, 1.0);
	normal = vec4(mat3(transpose(inverse(objPosMatrix[gl_InstanceID]))) * vNormal, 1.0);
	gl_Position = projectionMatrix * viewMatrix * vVerticPositionInModelCoordinate;
	TexCoord = aTexCoord;
	position = vVerticPositionInModelCoordinate.xyz;

	vec3 T = mat3(transpose(inverse(objPosMatrix[gl_InstanceID]))) * normalize(vTangent);
	vec3 N = normalize(normal.xyz);
	vec3 B = normalize(cross(T, N));
	TBN = mat3(T, B, N);

	cameraPos = cameraPosition;
}