#version 330 core
layout(location = 0) out vec4 Out0_color;
layout(location = 1) out vec4 Out1_color;

uniform sampler2D depthTexture;
uniform vec2 resolution;

uniform mat4 invProjection;

uniform float projectionScaleRadius;
uniform float stepsPerSlice;
uniform vec2 sliceCount;
uniform float thicknessHeuristic;
uniform float invRadiusSquared;
uniform float power;
uniform float invFarPlane;

/** @public-api */
#define PI                          3.14159265359
/** @public-api */
#define HALF_PI                     1.570796327

const float kLog2LodRate = 3.0;

#define saturate(x)                 clamp(x, 0.0, 1.0)

highp vec3 computeViewSpacePositionFromDepth(highp vec2 uv, highp float linearDepth,
        highp vec2 positionParams) {
    return vec3((0.5 - uv) * positionParams * linearDepth, linearDepth);
}

highp vec3 computeViewSpaceNormal(const highp vec2 uv,
        const highp vec3 position,
        highp vec2 texel, highp vec2 positionParams) {

    precision highp float;

    highp vec2 uvdx = uv + vec2(texel.x, 0.0);
    highp vec2 uvdy = uv + vec2(0.0, texel.y);
    vec3 px = computeViewSpacePositionFromDepth(uvdx,
            texture(depthTexture, uvdx).r, positionParams);
    vec3 py = computeViewSpacePositionFromDepth(uvdy,
            texture(depthTexture, uvdy).r, positionParams);
    vec3 dpdx = px - position;
    vec3 dpdy = py - position;
    return normalize(cross(dpdx, dpdy));;
}

highp vec3 getViewSpacePosition(vec2 uv, float level) {
    highp float depth = texture(depthTexture, uv).r;
    highp vec2 positionParams = vec2(invProjection[0][0], invProjection[1][1]);
    return computeViewSpacePositionFromDepth(uv, depth, positionParams);
}

float integrateArcCosWeight(float h, float n) {
    float arc = -cos(2.0 * h - n) + cos(n) + 2.0 * h * sin(n);
    return 0.25 * arc;
}

float spatialDirectionNoise(vec2 uv) {
    ivec2 position = ivec2(uv * resolution.xy);
	return (1.0/16.0) * (float(((position.x + position.y) & 3) << 2) + float(position.x & 3));
}

float spatialOffsetsNoise(vec2 uv) {
	ivec2 position = ivec2(uv * resolution.xy);
	return 0.25 * float((position.y - position.x) & 3);
}

float acosFast(float x) {
    float y = abs(x);
    float p = -0.1565827 * y + 1.570796;
    p *= sqrt(1.0 - y);
    return x >= 0.0 ? p : 3.14 - p;
}

void groundTruthAmbientOcclusion(out float obscurance, highp vec2 uv, highp vec3 origin, vec3 normal) {
    vec2 uvSamplePos = uv;
    highp vec3 viewDir = normalize(-origin);
    highp float ssRadius = -(projectionScaleRadius / origin.z);

    float noiseOffset = spatialOffsetsNoise(uv);
    float noiseDirection = spatialDirectionNoise(uv);

    float initialRayStep = fract(noiseOffset);

    // The distance we want to move forward for each step
    float stepRadius = ssRadius / (stepsPerSlice + 1.0);

    float visibility = 0.0;
    for (float i = 0.0; i < sliceCount.x; i += 1.0) {
        float slice = (i + noiseDirection) * sliceCount.y;
        float phi = slice * 3.14;
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);
        vec2 omega = vec2(cosPhi, sinPhi);

        // Calculate the direction of the current slice
        vec3 direction = vec3(cosPhi, sinPhi, 0.0);
        // Project direction onto the plane orthogonal to viewDir.
        vec3 orthoDirection = normalize(direction - (dot(direction, viewDir)*viewDir));
        // axis is orthogonal to direction and viewDir (basically the normal of the slice plane)
        // Used to define projectedNormal
        vec3 axis = cross(orthoDirection, viewDir);
        // Project the normal onto the slice plane
        vec3 projNormal = normal - axis * dot(normal, axis);

        float signNorm = sign(dot(orthoDirection, projNormal));
        float projNormalLength = length(projNormal);
        float cosNorm = clamp(dot(projNormal, viewDir) / projNormalLength, 0.0, 1.0);

        float n = signNorm * acosFast(cosNorm);

        float horizonCos0 = -1.0;
        float horizonCos1 = -1.0;
        for (float j = 0.0; j < stepsPerSlice; j += 1.0) {
            // At least move 1 pixel forward in the screen-space
            vec2 sampleOffset = max((j + initialRayStep)*stepRadius, 1.0 + j) * omega;
            float sampleOffsetLength = length(sampleOffset);

            //float level = clamp(floor(log2(sampleOffsetLength)) - kLog2LodRate, 0.0, float(materialParams.maxLevel));

            vec2 uvSampleOffset = sampleOffset * (1.0/resolution);

            vec2 sampleScreenPos0 = uv + uvSampleOffset;
            vec2 sampleScreenPos1 = uv - uvSampleOffset;

            // Sample the depth and use it to reconstruct the view space position
            highp vec3 samplePos0 = getViewSpacePosition(sampleScreenPos0, 0);
            highp vec3 samplePos1 = getViewSpacePosition(sampleScreenPos1, 0);

            highp vec3 sampleDelta0 = (samplePos0 - origin);
            highp vec3 sampleDelta1 = (samplePos1 - origin);
            highp vec2 sqSampleDist = vec2(dot(sampleDelta0, sampleDelta0), dot(sampleDelta1, sampleDelta1));
            vec2 invSampleDist = inversesqrt(sqSampleDist);

            // Use the view space radius to calculate the fallOff
            vec2 fallOff = clamp(sqSampleDist.xy * invRadiusSquared * 2.0, 0.0, 1.0);

            // sample horizon cos
            float shc0 = dot(sampleDelta0, viewDir) * invSampleDist.x;
            float shc1 = dot(sampleDelta1, viewDir) * invSampleDist.y;

            // If the new sample value is greater then the current one, update the value with some fallOff.
            // Otherwise, apply thicknessHeuristic.
            horizonCos0 = shc0 > horizonCos0 ? mix(shc0, horizonCos0, fallOff.x) : mix(horizonCos0, shc0, thicknessHeuristic);
            horizonCos1 = shc1 > horizonCos1 ? mix(shc1, horizonCos1, fallOff.y) : mix(horizonCos1, shc1, thicknessHeuristic);
        }

        float h0 = -acosFast(horizonCos1);
        float h1 = acosFast(horizonCos0);
        h0 = n + clamp(h0 - n, -HALF_PI, HALF_PI);
        h1 = n + clamp(h1 - n, -HALF_PI, HALF_PI);

        visibility += projNormalLength * (integrateArcCosWeight(h0, n) + integrateArcCosWeight(h1, n));
    }

    obscurance = 1.0 - saturate(visibility * sliceCount.y);
}

vec2 pack(highp float normalizedDepth) {
    // we need 16-bits of precision
    highp float z = clamp(normalizedDepth, 0.0, 1.0);
    highp float t = floor(256.0 * z);
    mediump float hi = t * (1.0 / 256.0);   // we only need 8-bits of precision
    mediump float lo = (256.0 * z) - t;     // we only need 8-bits of precision
    return vec2(hi, lo);
}

void main()
{
    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;
    highp float depth = texture(depthTexture, uv).r;
    highp vec2 positionParams = vec2(invProjection[0][0], invProjection[1][1]);
    highp vec3 origin = computeViewSpacePositionFromDepth(uv, depth, positionParams);

    vec3 normal = computeViewSpaceNormal(uv, origin, resolution, positionParams);

    float occlusion = 0.0;

    groundTruthAmbientOcclusion(occlusion, uv, origin, normal);

    float aoVisibility = pow(saturate(1.0 - occlusion), power);

    Out1_color.rgb = vec3(aoVisibility, pack(origin.z * invFarPlane));
}