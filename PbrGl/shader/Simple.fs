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