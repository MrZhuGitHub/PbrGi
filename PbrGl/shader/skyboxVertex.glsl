#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=1) in vec3 vNormal;
layout (location=2) in vec2 aTexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 cubeMapCoord;

void main()
{
    cubeMapCoord = vPosition;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
}