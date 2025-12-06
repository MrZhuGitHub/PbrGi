#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec2 Out1_color;

uniform vec2 axis;
uniform vec2 kernel[64];
uniform sampler2D depthTexture;
uniform vec2 resolution;
uniform int count;

void tap(inout highp vec4 sum, const float weight, const highp vec2 position) {
    highp vec4 s = texture(depthTexture, position);
    sum += s * weight;
}

void main()
{
    highp vec4 sum = vec4(0.0);
    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;

    tap(sum, kernel[0].x, uv);
    vec2 offset = axis;
    for (int i = 1; i < count; i++, offset += axis * 2.0) {
        float k = kernel[i].x;
        vec2 o = offset + axis * kernel[i].y;
        tap(sum, k, uv + o);
        tap(sum, k, uv - o);
    }
    
    Out1_color.xy = sum.xy;
}