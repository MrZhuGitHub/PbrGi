#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec2 Out1_color;

uniform float vsmExponent;
uniform float near;
uniform float far;

highp float LinearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{
    highp float depth = LinearizeDepth(gl_FragCoord.z);
    depth = (depth - near)/(far - near);
    depth = depth * 2.0 - 1.0;

    depth = exp(vsmExponent * depth);
    Out1_color.x = depth;
    Out1_color.y = depth * depth;
}