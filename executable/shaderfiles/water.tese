#version 460 core
#define PROCEDURAL_TERRAIN 1
#define M_PI 3.14159

layout(quads, fractional_even_spacing, cw) in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 tileSize;
uniform int gridW;
uniform int gridH;

uniform float noiseFreq;
uniform int noiseOctaves;
uniform vec2 translate;
uniform float heightScale;
uniform int noiseSize;
uniform float time;

//uniform sampler2D terrainTex;
uniform mediump sampler2D randTex;

uniform vec2 dir1;
uniform vec2 dir2;
uniform vec2 dir3;
uniform vec2 dir4;

uniform float wavelength1;
uniform float wavelength2;
uniform float wavelength3;
uniform float wavelength4;

uniform float amplitude1;
uniform float amplitude2;
uniform float amplitude3;
uniform float amplitude4;

uniform float speed1;
uniform float speed2;
uniform float speed3;
uniform float speed4;

in gl_PerVertex 
{
    vec4 gl_Position;
} gl_in[];

layout (location = 1) in block 
{
    mediump vec2 texCoord;
    vec2 tessLevelInner;
} In[];

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout (location = 1) out block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec3 normal;
    vec2 texCoords;
} Out;

void gerstnerWave(inout vec3 pos, inout vec3 normal)
{
    float wavelength[4] = float[](wavelength1, wavelength2, wavelength3, wavelength4);
    float amplitude[4] = float[](amplitude1, amplitude2, amplitude3, amplitude4);
    float speed[4] = float[](speed1, speed2, speed3, speed4);
    float steepness[4] = float[](1.0, 1.0, 1.0, 1.0);
    float numWaves = 4.0;
    vec2 direction_D[4] = vec2[](dir1, dir2, dir3, dir4);
    
    for(int i = 0; i < numWaves; i++)
    {
        float wi = 2.0 / wavelength[i];
        float WA = wi * amplitude[i];
        float Qi = steepness[i] / (amplitude[i] * wi * numWaves);
        float phi = speed[i] * wi;
        float rad = dot(direction_D[i], pos.xz) * wi + time * phi;

        pos.y += sin(rad) * amplitude[i];
        pos.xz += cos(rad) * amplitude[i] * direction_D[i] * Qi;
        normal.xz -= direction_D[i] * WA * cos(rad);
        normal.y -= Qi * WA * sin(rad);
        normal = normalize(normal);
    }
}

void main()
{
    vec3 pos = gl_in[0].gl_Position.xyz;
    pos.xz += gl_TessCoord.xy * tileSize.xz;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    gerstnerWave(pos, normal);
    pos.y += 0.1;
    Out.normal = normal;
    Out.vertex = pos;
    Out.vertex_viewspace = vec3(view * model * vec4(pos, 1));
    Out.texCoords = In[0].texCoord + (vec2(1.0 / gridW, 1.0 / gridH) * gl_TessCoord.xy);
    gl_Position = projection * view * model * vec4(pos, 1);
}