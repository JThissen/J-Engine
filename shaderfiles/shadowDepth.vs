#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out block
{
    vec2 TexCoords;
} Out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    Out.TexCoords = texCoords;
    gl_Position = position;
}