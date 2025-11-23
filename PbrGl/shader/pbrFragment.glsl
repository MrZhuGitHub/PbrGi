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

uniform bool roughnessTextureExist;
uniform sampler2D roughnessTexture;
uniform bool normalTextureExist;
uniform sampler2D normalTexture;

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

void main()
{
	float iblLuminance = 1.0;

	vec3 color;
	if (baseColorTextureExist) {
		color = texture(baseColorTexture, TexCoord).rgb;
	} else {
		color = baseColor;
	}

	//color.r = srgb_to_linear(color.r);
	//color.g = srgb_to_linear(color.g);
	//color.b = srgb_to_linear(color.b);

	float rough = roughness;
	float meta = metallic;

	//if (roughnessTextureExist) {
	//	meta = texture(roughnessTexture, TexCoord).r;
	//}

	vec3 view = normalize(cameraPos - position);

	vec3 n = normalize(normal.xyz);

	if (normalTextureExist) {
		vec3 result = texture(normalTexture, TexCoord).rgb;
		result = normalize(result * 2.0 - 1.0);   
		n = normalize(TBN * result);
	} 

	float NdotV = dot(n, view);

	vec3 diffuseColor = (1.0 - meta) * color.rgb;
	float reflectance = 0.1;
	vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - meta) + color * meta;

	vec3 dfg = textureLod(sampler0_iblDFG, vec2(abs(NdotV), rough), 0.0).rgb;
	vec3 splitsum = mix(dfg.xxx, dfg.yyy, f0);
	float lod = sampler0_iblSpecular_mipmapLevel * rough * (2.0 - rough);
	vec3 reflect = 2.0 * NdotV * n - view;
	vec3 Fr = splitsum * textureLod(sampler0_iblSpecular, reflect, lod).rgb;

	vec3 diffuseIrradiance = iblSH[0]
							+ iblSH[1] * (n.y) + iblSH[2] * (n.z) + iblSH[3] * (n.x)
							+ iblSH[4] * (n.y * n.x) + iblSH[5] * (n.y * n.z) + iblSH[6] * (3.0 * n.z * n.z - 1.0) + iblSH[7] * (n.z * n.x) + iblSH[8] * (n.x * n.x - n.y * n.y);

	vec3 Fd = diffuseColor * max(diffuseIrradiance, 0.0) * (1.0 - splitsum);

	vec3 hdrColor = iblLuminance * (Fd + Fr);

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