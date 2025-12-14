#version 330 core
layout(location = 1) out vec4 Out1_color;

uniform sampler2D depthTexture;

void main()
{
    Out1_color = vec4(1.0);
}