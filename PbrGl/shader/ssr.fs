#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec4 Out1_color;

uniform sampler2D depthTexture;
uniform sampler2D colorTexture;
uniform sampler2D normalTexture;

uniform mat4 uvFromViewMatrix;
uniform float near;
uniform float far;
uniform float vsZThickness;
uniform mat4 invProjection;

uniform vec2 resolution;

uniform float ssrBias;
uniform float maxRayTraceDistance;
uniform float fadeRateDistance;

highp vec4 mulMat4x4Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

void swap(inout highp float a, inout highp float b) {
     highp float temp = a;
     a = b;
     b = temp;
}

highp float distanceSquared(highp vec2 a, highp vec2 b) {
    a -= b;
    return dot(a, a);
}

highp float linearizeDepth(highp float depth) {

    const highp float preventDiv0 = 1.0 / 16777216.0;
    mat4 p = invProjection;

    return (depth * p[2].z + p[3].z) / max(depth * p[2].w + p[3].w, preventDiv0);
}

highp vec3 computeViewSpacePositionFromDepth(highp vec2 uv, highp float linearDepth,
        highp vec2 positionParams) {
    return vec3((0.5 - uv) * positionParams * linearDepth, linearDepth);
}

float interleavedGradientNoise(highp vec2 w) {
    const vec3 m = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(m.z * fract(dot(w, m.xy)));
}

bool traceScreenSpaceRay(highp vec3 vsOrigin, highp vec3 vsEndPoint, float jitterFraction, out highp vec2 hitPixel, out highp vec3 vsHitPoint) {
    highp vec4 H0 = mulMat4x4Float3(uvFromViewMatrix, vsOrigin);
    highp vec4 H1 = mulMat4x4Float3(uvFromViewMatrix, vsEndPoint);

    highp float k0 = 1.0 / H0.w;
    highp float k1 = 1.0 / H1.w;

    highp vec3 Q0 = vsOrigin * k0;
    highp vec3 Q1 = vsEndPoint * k1;

    highp vec2 P0 = H0.xy * k0;
    highp vec2 P1 = H1.xy * k1;

    hitPixel = vec2(-1.0, -1.0);

    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);

    highp vec2 delta = P1 - P0;

    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) {

        permute = true;

        delta = delta.yx;
        P1 = P1.yx;
        P0 = P0.yx;
    }

    float stepDirection = sign(delta.x);
    highp float invdx = stepDirection / delta.x;
    highp vec2 dP = vec2(stepDirection, invdx * delta.y);

    highp vec3  dQ = (Q1 - Q0) * invdx;
    highp float dk = (k1 - k0) * invdx;

    P0 += dP * jitterFraction; Q0 += dQ * jitterFraction; k0 += dk * jitterFraction;

    highp vec3  Q = Q0;
    highp float k = k0;

    highp float prevZMaxEstimate = vsOrigin.z;
    highp float stepCount = 0.0;
    highp float rayZMax = prevZMaxEstimate;
    highp float rayZMin = prevZMaxEstimate;
    highp float sceneZMax = rayZMax + 1e4;

    highp float end = P1.x * stepDirection;

    for (highp vec2 P = P0;
        ((P.x * stepDirection) <= end) &&
        ((hitPixel.x >= 0.0) && (hitPixel.x <= resolution.x) && (hitPixel.y >= 0.0) && (hitPixel.y <= resolution.y)) &&
        ((rayZMax < sceneZMax - vsZThickness) || (rayZMin > sceneZMax)) &&
        (sceneZMax != 0.0);
        P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0) {

        hitPixel = permute ? P.yx : P;

        rayZMin = prevZMaxEstimate;

        rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) { swap(rayZMin, rayZMax); }

        sceneZMax = linearizeDepth(texelFetch(depthTexture, ivec2(hitPixel), 0).r);
    } 

    Q.xy += dQ.xy * stepCount;
    vsHitPoint = Q * (1.0 / k);

    return (rayZMax >= sceneZMax - vsZThickness) && (rayZMin <= sceneZMax);
}

void main() {

    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;
    highp float depth = linearizeDepth(textureLod(depthTexture, uv, 0.0).r);
    highp vec2 positionParams = vec2(2.0*invProjection[0][0], 2.0*invProjection[1][1]);
    highp vec3 vsOrigin = computeViewSpacePositionFromDepth(uv, depth, positionParams);   

    highp vec3 normal = normalize(texture(normalTexture, uv).rgb);

    highp vec3 viewDirection = normalize(-vsOrigin);

	float NdotV = dot(normal, viewDirection);

    highp vec3 vsDirection = normalize(2.0 * NdotV * normal - viewDirection);

    vsOrigin = vsOrigin + ssrBias * vsDirection;

    float jitterFraction = interleavedGradientNoise(absolutePixelCoord);

    float t = vsDirection.z > 0.0 ? ((- near - vsOrigin.z)/vsDirection.z) : (-(vsOrigin.z + far)/vsDirection.z);
    highp vec3 vsEndPoint = vsOrigin + t * vsDirection;

    highp vec2 hitPixel;
    highp vec3 vsHitPoint;

    vec4 Fr = vec4(0.0);

    if (traceScreenSpaceRay(vsOrigin, vsEndPoint, jitterFraction, hitPixel, vsHitPoint)) {
        float t = distance(vsOrigin, vsHitPoint) / maxRayTraceDistance;
        float fade = clamp((fadeRateDistance - fadeRateDistance * t), 0.0, 1.0);
        fade *= (1.0 - max(0.0, vsDirection.z));
        Fr = vec4(textureLod(colorTexture, hitPixel/resolution, 0.0).rgb * fade, fade);
    }

    Out1_color = Fr;
}