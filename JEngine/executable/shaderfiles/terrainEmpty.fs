#version 460 core

layout (location = 1) in block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec4 vertex_light_clipspace;
    vec3 normal;
    mediump vec2 texCoords;
} In;

void main()
{
    gl_FragDepth = gl_FragCoord.z;
}