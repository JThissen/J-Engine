#version 460 core

layout (location = 0) in vec4 vertex;
layout (location = 1) out block 
{
    mediump vec2 texCoord;
} Out;

out gl_PerVertex
{
    vec4 gl_Position;
};

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 tileSize;
uniform int gridW;
uniform int gridH;
uniform int outerTessFactor;
uniform int innerTessFactor;

uniform float noiseFreq;
uniform int noiseOctaves;
uniform vec2 translate;
uniform float heightScale;
uniform int noiseSize;

void main()
{
    vec3 gridOrigin = vec3(-tileSize.x * gridW * 0.5f, 0.0f, -tileSize.z * gridH * 0.5);

    int ix = gl_InstanceID % gridW;
    int iz = gl_InstanceID / gridH;

    Out.texCoord = vec2(float(ix) / float(gridW), float(iz) / float(gridH));

    vec3 pos = gridOrigin + vec3(float(ix)*tileSize.x, 0, float(iz)*tileSize.z);
    gl_Position = vec4(pos, 1.0f);
}