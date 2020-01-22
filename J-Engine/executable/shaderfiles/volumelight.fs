#version 460 core
 
in vec2 inoutTexCoord;
out vec4 outColor;

uniform vec2 lightScreenPos;
uniform sampler2D tex1;
uniform sampler2D tex2;

uniform int samples;
uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;

void main(void)
{
	outColor = vec4(0);
	vec2 textCoo = inoutTexCoord.xy;
	vec2 deltaTextCoord = textCoo - lightScreenPos;
 	deltaTextCoord *= 1.0 /  float(samples) * density;
	float illuminationDecay = 1.0;

	for(int i=0; i < samples ; i++)
  	{
		textCoo -= deltaTextCoord;
		vec4 colorSample = vec4(texture(tex1, textCoo).rgb, 1.0);
		colorSample  *= illuminationDecay * weight;
		outColor += colorSample;
		illuminationDecay *= decay;
 	}

	outColor *= exposure;
	outColor += vec4(texture( tex2, inoutTexCoord).rgb, 1.0);
	outColor *= 0.5;

	//outColor = vec4(texture( tex1, inoutTexCoord).rgb, 1.0);
}