#version 460 core
#define PROCEDURAL_TERRAIN 1

layout(quads, fractional_even_spacing, cw) in;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightProjection;
uniform mat4 lightView;
uniform vec3 tileSize;
uniform int gridW;
uniform int gridH;

uniform float noiseFreq;
uniform int noiseOctaves;
uniform vec2 translate;
uniform float heightScale;
uniform int noiseSize;

uniform mediump sampler2D randTex;

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
    vec4 vertex_light_clipspace;
    vec3 normal;
    mediump vec2 texCoords;
} Out;

//NVIDIA FRACTAL BROWNIAN MOTION NOISE

// derivative of fade function
vec2 dfade(vec2 t)
{
    return 30.0*t*t*(t*(t-2.0)+1.0); // new curve (quintic)
}

// smooth interpolation curve
vec2 fade(vec2 t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); // new curve (quintic)
}

// 2D noise with derivatives
// returns derivative in xy, normal noise value in z
// http://www.iquilezles.org/www/articles/morenoise/morenoise.htm

vec3 dnoise(vec2 p)
{
    float invNoiseSize = 1.0 / noiseSize;

    vec2 i = floor(p);
    vec2 u = p - i;

    vec2 du = dfade(u);
    u = fade(u);

    // get neighbouring noise values
    vec4 n = textureGather(randTex, i * invNoiseSize)*2.0-1.0;

    // rename components for convenience
    float a = n.w;
    float b = n.z;
    float c = n.x;
    float d = n.y;

    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = a - b - c + d;

    vec3 r;
    // noise derivative
    r.xy = (vec2(k1, k2) + k3*u.yx) * du;
    // noise value
    r.z = k0 + k1*u.x + k2*u.y + k3*u.x*u.y;
    return r;
}

float fBmGrad(vec2 p, int octaves, float lacunarity, float gain)
{
    const mat2 rotateMat = mat2(1.6, -1.2, 1.2, 1.6);
    float amp = 0.5;
    vec2 d = vec2(0.0);
    float sum = 0.0;
    for(int i=0; i<octaves; i++) {
        vec3 n = dnoise(p);
        d += n.xy;
        sum += n.z*amp / (1.0 + dot(d, d));   // sum scaled by gradient
        amp *= gain;
        //p *= lacunarity;
        p = rotateMat * p;
    }
    return sum;
}
// END NVIDIA FRACTAL BROWNIAN MOTION NOISE

float terrain(vec2 p)
{
    p += translate;
    p *= noiseFreq;
    float h = fBmGrad(p, noiseOctaves, 2.0, 0.5) + 0.2;
    h *= heightScale;
    const float waterLevel = 0.05;
    float land = smoothstep(waterLevel, waterLevel+0.2, h);
    // scale down terrain at shore
    h *= 0.1 + 0.9*land;

    return h;
}

void main()
{
    vec3 pos = gl_in[0].gl_Position.xyz;
    pos.xz += gl_TessCoord.xy * tileSize.xz;

#if PROCEDURAL_TERRAIN
    float h = terrain(pos.xz);
    vec3 n = vec3(0, 1, 0);
    pos.y = h;

    //normal per vertex
    vec2 triSize = tileSize.xz / In[0].tessLevelInner;
    vec3 pos_dx = pos.xyz + vec3(triSize.x, 0.0, 0.0);
    vec3 pos_dz = pos.xyz + vec3(0.0, 0.0, triSize.y);
    pos_dx.y = terrain(pos_dx.xz);
    pos_dz.y = terrain(pos_dz.xz);
    n = normalize(cross(pos_dz - pos.xyz, pos_dx - pos.xyz));
#else
    // read from pre-calculated texture
    // vec2 uv = In[0].texCoord + (vec2(1.0 / gridW, 1.0 / gridH) * gl_TessCoord.xy);
    // vec4 t = texture2D(terrainTex, uv);
    // float h = t.w;
    // //pos.y = t.w;
    // pos.y = t.r * 5.0f;
    // vec3 n = t.xyz;
#endif

    Out.normal = n;
    Out.vertex = pos;
    Out.vertex_viewspace = vec3(view * model * vec4(pos, 1));
    Out.vertex_light_clipspace = lightProjection * lightView * model * vec4(pos, 1);
    Out.texCoords = In[0].texCoord + (vec2(1.0 / gridW, 1.0 / gridH) * gl_TessCoord.xy);
    gl_Position = projection * view * model * vec4(pos, 1);
    //gl_ClipDistance[0] = 0;
}