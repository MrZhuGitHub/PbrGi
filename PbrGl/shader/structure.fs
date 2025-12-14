#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec2 Out1_color;

uniform float near;
uniform float far;

in highp float depth;

highp float LinearizeDepth(float depth)
{
    float ndc = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}

void main()
{
    highp float remapDepth = ((-depth) - near)/(far - near);
    remapDepth = remapDepth * 2.0 - 1.0;

    Out1_color.x = depth;
    Out1_color.y = remapDepth;
}