#version 460 core

out vec4 FragColor;

in block
{
    vec2 TexCoords;
} In;

uniform sampler2D positionBuffer;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPosition;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}