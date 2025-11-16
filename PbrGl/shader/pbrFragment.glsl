
// //precompute specular
// uniform sampler2D  sampler0_iblDFG;
// uniform samplerCube sampler0_iblSpecular;
// //precompute diffuse
// uniform float iblSH[9];
// //precompute mapHDR2LDR
// uniform sampler3D lut;
// uniform vec2 lutSize;

// vec3 linear_to_LogC(const vec3 x) {
//     // Alexa LogC EI 1000
//     const float a = 5.555556;
//     const float b = 0.047996;
//     const float c = 0.244161 / log2(10.0);
//     const float d = 0.386036;
//     return c * log2(a * x + b) + d;
// }

// void evaluateIBL(vec3 baseColor, float roughness, float metallic, float reflectance) {
//     //PBR
//     vec3 diffuseColor = (1.0 - metallic) * baseColor.rgb;
//     vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;

//     vec3 dfg = textureLod(sampler0_iblDFG, vec2(NoV, roughness), 0.0).rgb;
//     vec3 E = mix(dfg.xxx, dfg.yyy, f0);

//     float lod = perceptualRoughnessToLod(perceptualRoughness);
//     vec3 r = getReflectedVector(normal, light);
//     vec3 Fr = E * textureLod(sampler0_iblSpecular, r, lod);

//     vec3 diffuseIrradiance = iblSH[0]
//                             + iblSH[1] * (n.y) + iblSH[2] * (n.z) + iblSH[3] * (n.x)
//                             + iblSH[4] * (n.y * n.x) + iblSH[5] * (n.y * n.z) + iblSH[6] * (3.0 * n.z * n.z - 1.0) + iblSH[7] * (n.z * n.x) + iblSH[8] * (n.x * n.x - n.y * n.y);
//     vec3 diffuseBRDF = 1.0/PI;
//     vec3 Fd = diffuseColor * diffuseIrradiance * (1.0 - E) * diffuseBRDF;
//     vec3 hdrColor = Fr + Fd;

//     //ToneMap
//     vec3 v = LogC_to_linear(hdrColor.rgb);
//     v = lutSize.x + v * lutSize.y;
//     vec3 LdrColor = textureLod(lut, v, 0.0).rgb;
// }

#version 330 core
layout(location = 0) out vec4 Out0_color;
in vec4 normal;
in vec2 TexCoord;
in vec3 position;

uniform bool baseColorTextureExist;
uniform sampler2D baseColorTexture;
uniform vec3 baseColor;
uniform float opacityFactor;
uniform float metallic;
uniform float roughness;

uniform vec3 iblSH[9];
uniform sampler2D  sampler0_iblDFG;
uniform samplerCube sampler0_iblSpecular;
uniform vec3 cameraPosition;
uniform float sampler0_iblSpecular_mipmapLevel;

void main()
{
	vec3 color;
	if (baseColorTextureExist) {
		color = texture(baseColorTexture, TexCoord).rgb;
	} else {
		color = baseColor;
	}

	vec3 view = normalize(cameraPosition - position);
	vec3 n = normalize(normal.xyz);

	float NdotV = max(dot(n, view), 0.0);

	vec3 diffuseColor = (1.0 - metallic) * color.rgb;
	float reflectance = 0.0;
	vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + color * metallic;

	vec3 dfg = textureLod(sampler0_iblDFG, vec2(NdotV, roughness * roughness), 0.0).rgb;
	vec3 splitsum = mix(dfg.xxx, dfg.yyy, f0);

	float lod = sampler0_iblSpecular_mipmapLevel * roughness * roughness;
	vec3 reflect = 2.0 * NdotV * n - view;
	vec3 Fr = splitsum * textureLod(sampler0_iblSpecular, reflect, lod).rgb;

	vec3 diffuseIrradiance = iblSH[0]
							+ iblSH[1] * (n.y) + iblSH[2] * (n.z) + iblSH[3] * (n.x)
							+ iblSH[4] * (n.y * n.x) + iblSH[5] * (n.y * n.z) + iblSH[6] * (3.0 * n.z * n.z - 1.0) + iblSH[7] * (n.z * n.x) + iblSH[8] * (n.x * n.x - n.y * n.y);
	vec3 diffuseBRDF = vec3(1.0/3.14);
	vec3 Fd = diffuseColor * diffuseIrradiance * (1.0 - splitsum) * diffuseBRDF;

	vec3 hdrColor = Fr + Fd;

    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;	

	vec3 LDR = (hdrColor * (A * hdrColor + B)) / (hdrColor * (C * hdrColor + D) + E);
	LDR = pow(LDR, vec3(1.0/2.2));
	Out0_color = vec4(LDR, opacityFactor);
}