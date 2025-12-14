#version 330 core
layout (location=0) in vec3 vPosition;
layout (location=2) in vec2 aTexCoord;

void main()
{
	gl_Position = vec4(vPosition, 1.0);
}