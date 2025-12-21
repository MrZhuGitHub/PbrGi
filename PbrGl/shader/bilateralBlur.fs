#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec4 Out1_color;

uniform sampler2D aoTexture;

uniform vec2 resolution;
uniform float kernel[16];
uniform vec2 axis;
uniform int sampleCount;
uniform float farPlaneOverEdgeDistance;

highp float unpack(highp vec2 depth) {
    return (depth.x * (256.0 / 257.0) + depth.y * (1.0 / 257.0));
}

float bilateralWeight(in highp float depth, in highp float sampleDepth) {
    float diff = (sampleDepth - depth) * farPlaneOverEdgeDistance / depth;
    return max(0.0, 1.0 - diff * diff);
}

void tap(inout float sum, inout float totalWeight, float weight, float depth, vec2 position) {
    vec3 data = textureLod(aoTexture, position, 0.0).rgb;

    float bilateral = weight * bilateralWeight(depth, unpack(data.gb));
    sum += data.r * bilateral;
    totalWeight += bilateral;
}

float interleavedGradientNoise(highp vec2 w) {
    const vec3 m = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(m.z * fract(dot(w, m.xy)));
}

void main()
{
    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;

    vec3 data = textureLod(aoTexture, uv, 0.0).rgb;
    if(data.g * data.b == 1.0) {
        Out1_color.rgb = data;
        return;
    }

    float depth = unpack(data.gb);
    float totalWeight = kernel[0];
    float sum = data.r * totalWeight;

    vec2 offset = axis;
    for (int i = 1; i < sampleCount; i++) {
        float weight = kernel[i];
        tap(sum, totalWeight, weight, depth, uv + offset);
        tap(sum, totalWeight, weight, depth, uv - offset);
        offset += axis;
    }

    float ao = sum * (1.0 / totalWeight);

    ao += ((interleavedGradientNoise(gl_FragCoord.xy) - 0.5) / 255.0);

    Out1_color.r = ao;
    Out1_color.gb = data.gb;
}