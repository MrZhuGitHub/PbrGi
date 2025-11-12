
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
uniform bool baseColorTextureExist;
uniform sampler2D baseColorTexture;
uniform vec3 baseColor;
uniform float opacityFactor;
void main()
{
	vec3 n = normalize(normal.xyz);
	float index = 0.3;
	index = max(dot(n, vec3(1.0, 0.0, 0.0)), index);
	index = max(dot(n, vec3(0.0, 1.0, 0.0)), index);
	index = max(dot(n, vec3(0.0, 0.0, 1.0)), index);
	vec3 color;
	if (baseColorTextureExist) {
		color = texture(baseColorTexture, TexCoord).rgb;
	} else {
		color = baseColor;
	}
	float gamma = 2.2;
	Out0_color = vec4(pow(index * color, vec3(1.0 / gamma)), opacityFactor);
}