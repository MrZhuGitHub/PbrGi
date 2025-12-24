#version 330 core
layout(location = 0) out vec4 Out0_color;

uniform vec2 resolution;
uniform sampler2D hdrTexture;

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
    vec2 absolutePixelCoord = gl_FragCoord.xy;
    highp vec2 uv = absolutePixelCoord/resolution;

    highp vec4 hdrColor = texture(hdrTexture, uv);

    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;	
    vec3 LDR = (hdrColor.rgb * (A * hdrColor.rgb + B)) / (hdrColor.rgb * (C * hdrColor.rgb + D) + E);

    LDR.r = linear_to_srgb(LDR.r);
    LDR.g = linear_to_srgb(LDR.g);
    LDR.b = linear_to_srgb(LDR.b);

    Out0_color = vec4(LDR, hdrColor.a);
}
