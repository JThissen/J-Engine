#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomblur;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomblur, TexCoords).rgb;
    hdrColor += bloomColor;
    vec3 tonemapped = vec3(1.0) - exp(-hdrColor * exposure);
    vec3 gammaCorrection = pow(tonemapped, vec3(1.0 / gamma));
    FragColor = vec4(hdrColor, 1.0);
}