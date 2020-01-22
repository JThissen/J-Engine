#version 460 core


layout (vertices = 1) out;

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
uniform vec4 frustumPlanes[6];
uniform bool cameraLOD;
uniform float minDepth;
uniform float maxDepth;
uniform float minTessellation;
uniform float maxTessellation;


in gl_PerVertex 
{
    vec4 gl_Position;
} gl_in[];

layout (location = 1) in block 
{
    mediump vec2 texCoord;
} In[];


out gl_PerVertex 
{
    vec4 gl_Position;
} gl_out[];

layout (location = 1) out block 
{
    mediump vec2 texCoord;
    vec2 tessLevelInner;
} Out[];

void main() 
{
    bool cull = true;
    bool lod = true;
    vec3 halfTileSize = vec3(tileSize.x, heightScale, tileSize.z) * 0.5;
    float tileBoundingSphereR = length(halfTileSize);

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    Out[gl_InvocationID].texCoord = In[gl_InvocationID].texCoord;

    if(cameraLOD)
    {
        vec4 position_projspace = projection * view * model * gl_in[gl_InvocationID].gl_Position;
        float depth = clamp((abs(position_projspace.z) - minDepth) / (maxDepth - minDepth), 0.0, 1.0);
        float tessLevel = mix(maxTessellation, minTessellation, depth);

        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
        gl_TessLevelOuter[3] = tessLevel;

        gl_TessLevelInner[0] = tessLevel;
        gl_TessLevelInner[1] = tessLevel;

        Out[gl_InvocationID].tessLevelInner = vec2(tessLevel);
    }
    else
    {
        gl_TessLevelOuter[0] = outerTessFactor;
        gl_TessLevelOuter[1] = outerTessFactor;
        gl_TessLevelOuter[2] = outerTessFactor;
        gl_TessLevelOuter[3] = outerTessFactor;

        gl_TessLevelInner[0] = innerTessFactor;
        gl_TessLevelInner[1] = innerTessFactor;

        Out[gl_InvocationID].tessLevelInner = vec2(innerTessFactor);
    }
}

