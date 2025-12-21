#version 330 core
layout(location = 0) out vec4 Out0_color;
in vec4 normal;
in vec2 TexCoord;
in vec3 position;
in mat3 TBN;
in vec3 cameraPos;

uniform bool baseColorTextureExist;
uniform sampler2D baseColorTexture;
uniform vec3 baseColor;
uniform float opacityFactor;
uniform float metallic;
uniform float roughness;

uniform vec3 iblSH[9];
uniform sampler2D  sampler0_iblDFG;
uniform samplerCube sampler0_iblSpecular;
uniform float sampler0_iblSpecular_mipmapLevel;

uniform bool metalnessTextureExist;
uniform sampler2D metalnessTexture;
uniform bool roughnessTextureExist;
uniform sampler2D roughnessTexture;
uniform bool normalTextureExist;
uniform sampler2D normalTexture;
uniform bool emissionTextureExist;
uniform sampler2D emissionTexture;

uniform float clearCoat;
uniform float clearCoatRoughness;

uniform bool shadowMapExist;
uniform sampler2D shadowMapTexture;
uniform mat4 lightCameraViewMatrix;
uniform mat4 lightCameraProjectionMatrix;
uniform float vsmExponent;
uniform float near;
uniform float far;
uniform float vsmDepthScale;
uniform float vsmLightBleedReduction;

uniform bool unLight;
uniform vec3 unLightColor;

uniform bool aoTextureExist;
uniform sampler2D aoTexture;

uniform vec2 resolution;

#define PI                          3.14159265359
#define saturate(x)                 clamp(x, 0.0, 1.0)


float linstep(const float min, const float max, const float v) {
    // we could use smoothstep() too
    return clamp((v - min) / (max - min), 0.0, 1.0);
}

float reduceLightBleed(const float pMax, const float amount) {
    // Remove the [0, amount] tail and linearly rescale (amount, 1].
    return linstep(amount, 1.0, pMax);
}

float chebyshevUpperBound(const highp vec2 moments, const highp float mean,
        const highp float minVariance, const float lightBleedReduction) {

    highp float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance);

    highp float d = mean - moments.x;
    float pMax = variance / (variance + d * d);

    pMax = reduceLightBleed(pMax, lightBleedReduction);

    return mean <= moments.x ? 1.0 : pMax;
}

float evaluateShadowVSM(const highp vec2 moments, const highp float depth) {
    highp float depthScale = vsmDepthScale * depth;
    highp float minVariance = depthScale * depthScale;
    return chebyshevUpperBound(moments, depth, minVariance, vsmLightBleedReduction);
}

float F_Schlick(float f0, float f90, float VoH) {
    return f0 + (f90 - f0) * pow(1.0 - VoH, 5);
}

vec3 f0ClearCoatToSurface(const vec3 f0) {
    // Approximation of iorTof0(f0ToIor(f0), 1.5)
    // This assumes that the clear coat layer has an IOR of 1.5
	vec3 newf0 = f0 * (f0 * (0.941892 - 0.263008 * f0) + 0.346479) - 0.0285998;
	newf0 = clamp(newf0, 0.0, 1.0);
	return newf0;
}

float srgb_to_linear(float c) {
    if (c <= 0.04045) {
        return c / 12.92;
    } else {
        return pow((c + 0.055) / 1.055, 2.4);
    }
}

float linear_to_srgb(float c) {
    float a  = 0.055;
    float a1 = 1.055;
    float b  = 12.92;
    float p  = 1.0 / 2.4;
	if (c < 0.0031308) {
		c = c * b;
	} else {
		c = a1 * pow(c, p) - a;
	}

    return c;
}

bool isTexCoordValid(vec2 texCoord) {
    return all(greaterThanEqual(texCoord, vec2(0.0))) && 
           all(lessThanEqual(texCoord, vec2(0.99)));
}

float acosFast(float x) {
    // Lagarde 2014, "Inverse trigonometric functions GPU optimization for AMD GCN architecture"
    // This is the approximation of degree 1, with a max absolute error of 9.0x10^-3
    float y = abs(x);
    float p = -0.1565827 * y + 1.570796;
    p *= sqrt(1.0 - y);
    return x >= 0.0 ? p : PI - p;
}

/**
 * Approximates acos(x) with a max absolute error of 9.0x10^-3.
 * Valid only in the range 0..1.
 */
float acosFastPositive(float x) {
    float p = -0.1565827 * x + 1.570796;
    return p * sqrt(1.0 - x);
}

float sq(float x) {
    return x * x;
}

float sphericalCapsIntersection(float cosCap1, float cosCap2, float cosDistance) {

    float r1 = acosFastPositive(cosCap1);
    float r2 = acosFastPositive(cosCap2);
    float d  = acosFast(cosDistance);


    if (min(r1, r2) <= max(r1, r2) - d) {
        return 1.0 - max(cosCap1, cosCap2);
    } else if (r1 + r2 <= d) {
        return 0.0;
    }

    float delta = abs(r1 - r2);
    float x = 1.0 - saturate((d - delta) / max(r1 + r2 - delta, 1e-4));

    float area = sq(x) * (-2.0 * x + 3.0);
    return area * (1.0 - max(cosCap1, cosCap2));
}

float SpecularAoCones(float NdotV, float visibility, float roughness) {

    float cosAv = sqrt(1.0 - visibility);
    float cosAs = exp2(-3.321928 * sq(roughness));
    float cosB  = NdotV;

    float ao = sphericalCapsIntersection(cosAv, cosAs, cosB) / (1.0 - cosAs);

    return mix(1.0, ao, smoothstep(0.01, 0.09, roughness));
}

void main()
{
    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;

	if (unLight) {
		float visibility = 1.0;
		if (shadowMapExist) {
			highp vec4 p1 = lightCameraViewMatrix * vec4(position, 1.0);
			highp float depth = (-p1.z - near)/(far - near);
			if (depth <= 1.0 && depth >= 0) {
				depth = depth * 2.0 - 1.0;
				depth = vsmExponent * depth;
				depth = exp(depth);

				highp vec4 p2 = lightCameraProjectionMatrix * p1;
				p2 = 0.5*(1.0 + p2/p2.w);
				highp vec2 moments = texture(shadowMapTexture, p2.xy).xy;

				if (isTexCoordValid(p2.xy)) {
					visibility = evaluateShadowVSM(moments.xy, depth);
				}
			}
		}
		
		Out0_color = vec4(unLightColor * (0.7 + visibility * 0.3), 1.0);

	} else {
		float iblLuminance = 1.0;

		vec3 color = baseColor;;
		if (baseColorTextureExist) {
			color = texture(baseColorTexture, TexCoord).rgb;
			color.r = srgb_to_linear(color.r);
			color.g = srgb_to_linear(color.g);
			color.b = srgb_to_linear(color.b);
		}

		float rough = roughness;
		if (roughnessTextureExist) {
			rough = texture(roughnessTexture, TexCoord).g;
		}


		float meta = metallic;
		if (metalnessTextureExist) {
			meta = texture(metalnessTexture, TexCoord).b;
		}

		vec3 view = normalize(cameraPos - position);

		vec3 n = normalize(normal.xyz);

		if (normalTextureExist) {
			vec3 result = texture(normalTexture, TexCoord).rgb;
			result = normalize(result * 2.0 - 1.0);   
			n = normalize(TBN * result);
		} 

		float NdotV = dot(n, view);

		float diffuseAO = 1.0;
		float specularAO = 1.0;
		if (aoTextureExist) {
			diffuseAO = texture(aoTexture, uv).r;
			specularAO = SpecularAoCones(abs(NdotV), diffuseAO, rough);
		}

		vec3 diffuseColor = (1.0 - meta) * color.rgb;
		float reflectance = 0.5;
		vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - meta) + color * meta;

		//f0 = mix(f0, f0ClearCoatToSurface(f0), clearCoat);

		vec3 dfg = textureLod(sampler0_iblDFG, vec2(abs(NdotV), 1.0 - rough), 0.0).rgb;
		vec3 splitsum = mix(dfg.xxx, dfg.yyy, f0);
		float lod = sampler0_iblSpecular_mipmapLevel * rough * (2.0 - rough);
		vec3 reflect = 2.0 * NdotV * n - view;
		vec3 Fr = splitsum * textureLod(sampler0_iblSpecular, reflect, lod).rgb * specularAO;

		vec3 diffuseIrradiance = iblSH[0]
								+ iblSH[1] * (n.y) + iblSH[2] * (n.z) + iblSH[3] * (n.x)
								+ iblSH[4] * (n.y * n.x) + iblSH[5] * (n.y * n.z) + iblSH[6] * (3.0 * n.z * n.z - 1.0) + iblSH[7] * (n.z * n.x) + iblSH[8] * (n.x * n.x - n.y * n.y);

		vec3 Fd = diffuseColor * max(diffuseIrradiance, 0.0) * (1.0 - splitsum) * diffuseAO;

		//clear coat
		float Fc = F_Schlick(0.04, 1.0, abs(NdotV)) * clearCoat;
		Fd = (1.0 - Fc) * Fd;
		Fr = (1.0 - Fc) * Fr;

		float clearCoatlod = sampler0_iblSpecular_mipmapLevel * clearCoatRoughness * (2.0 - clearCoatRoughness);
		Fr += textureLod(sampler0_iblSpecular, reflect, clearCoatlod).rgb * Fc;

		vec3 hdrColor = iblLuminance * (Fd + Fr);


		float visibility = 1.0;
		// if (shadowMapExist) {
		// 	highp vec4 p1 = lightCameraViewMatrix * vec4(position, 1.0);
		// 	highp float depth = (-p1.z - near)/(far - near);
		// 	if (depth <= 1.0 && depth >= 0) {
		// 		depth = depth * 2.0 - 1.0;
		// 		depth = vsmExponent * depth;
		// 		depth = exp(depth);

		// 		highp vec4 p2 = lightCameraProjectionMatrix * p1;
		// 		p2 = 0.5*(1.0 + p2/p2.w);
		// 		highp vec2 moments = texture(shadowMapTexture, p2.xy).xy;

		// 		if (isTexCoordValid(p2.xy)) {
		// 			visibility = evaluateShadowVSM(moments.xy, depth);
		// 		}
		// 	}
		// }
		hdrColor = (visibility * 0.7 + 0.3) * hdrColor;

		if (emissionTextureExist) {
			hdrColor = hdrColor + texture(emissionTexture, TexCoord).rgb;
		}

		const float A = 2.51;
		const float B = 0.03;
		const float C = 2.43;
		const float D = 0.59;
		const float E = 0.14;	
		vec3 LDR = (hdrColor * (A * hdrColor + B)) / (hdrColor * (C * hdrColor + D) + E);

		LDR.r = linear_to_srgb(LDR.r);
		LDR.g = linear_to_srgb(LDR.g);
		LDR.b = linear_to_srgb(LDR.b);

		Out0_color = vec4(LDR, opacityFactor);
	}

}