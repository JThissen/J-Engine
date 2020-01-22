#version 460 core

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTexCoord;
out vec2 TexCoords;

out position_block
{
    vec4 pos_worldspace;
    vec4 pos_viewspace;
} Out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    Out.pos_viewspace = inverse(projection) * inPosition;
    Out.pos_viewspace /= Out.pos_viewspace.w;
    Out.pos_worldspace = inverse(view * model) * Out.pos_viewspace;

	gl_Position = inPosition;
    TexCoords = inTexCoord;
}