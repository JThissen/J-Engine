#version 460 core

layout (location = 1) in block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec4 vertex_light_clipspace;
    vec3 normal;
    mediump vec2 texCoords;
} In;

layout (location = 4) out vec4 occlusionColor;
layout (location = 2) out vec4 position;
layout (location = 1) out vec3 normal;
layout (location = 0) out vec4 fragColor;

uniform sampler2D grassSampler;
uniform sampler2D snowSampler;
uniform sampler2D sandSampler;
uniform sampler2D cliffSampler;
uniform sampler2D shadowDepthBuffer;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float fogExp;
uniform float heightScale;
uniform float de;
uniform float di;
uniform float fogHeight;
uniform float fogUpperBound;
uniform float fogLowerBound;
uniform vec3 fogColor;
uniform bool toggleGrid;
uniform float lineWidth;
uniform bool showSSAO;
uniform vec3 sunPosition;
uniform vec3 lightColor;
uniform float diffuseAmplitude;
uniform float snowBreakpoint;
uniform float shadowBias;
uniform float shadowAlpha;
uniform float shadowSamples;

noperspective in vec3 edgeDistance;
float near = 0.1; 
float far  = 100.0; 

float saturate(float v) 
{
    return clamp( v, 0.0, 1.0);
}

vec3 blendRGB(vec3 color1, vec3 color2, vec3 color3, float value) 
{
	return (color1 * value) + (color2 * value) + (color3 * value);
}

float calcShadow(vec4 vertex_light_clipspace, vec3 normal, vec3 lightDir)
{
    vec3 vertex_light_ndcspace = vertex_light_clipspace.xyz / vertex_light_clipspace.w;
    vec3 vertex_light_depthspace = vertex_light_ndcspace * 0.5 + 0.5; // to [0:1]
    float shadowbuffer_depth = texture(shadowDepthBuffer, vertex_light_depthspace.xy).r;
    float vertex_depth = vertex_light_depthspace.z;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowDepthBuffer, 0);

    float numberOfSamples = shadowSamples;
    float sampleBounds = (numberOfSamples - 1.0) / 2.0;

    for(float x = -sampleBounds; x <= sampleBounds; x += 1.0)
    {
        for(float y = -sampleBounds; y <= sampleBounds; y += 1.0)
        {
            float pcfDepth = texture(shadowDepthBuffer, vertex_light_depthspace.xy + vec2(x, y) * texelSize).r;
            shadow += vertex_depth - shadowBias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / (numberOfSamples * numberOfSamples);
}

vec3 shadeTerrain(vec3 vertex, vec3 vertex_viewspace, vec3 normal, vec2 texCoords, vec4 vertex_light_clipspace)
{
    //textures
    float height = vertex.y;
    vec3 waterColor = vec3(0.2, 0.4, 0.5 );

    vec4 grassTexture = vec4(texture(grassSampler, texCoords * 10, 0).rgb * 1, 1.0);
	vec4 grassLower = vec4(texture(grassSampler, texCoords * 20 + 20, 0).rgb * 1, 1.0);
	vec4 grassUpper = vec4(texture(grassSampler, texCoords * 30 + 30, 0).rgb * 1, 1.0);
	vec4 grassDetiled = vec4(blendRGB(grassTexture.rgb, grassLower.rgb, grassUpper.rgb, 0.333), 1.0);

    vec4 snowUpper = vec4(texture(snowSampler, texCoords * 20).rgb * 1.0, 1.0);
	vec4 snowMid = vec4(texture(snowSampler, texCoords * 30 + 30).rgb * 1.0, 1.0);
	vec4 snowLower = vec4(texture(snowSampler, texCoords * 40 + 40).rgb * 1.0, 1.0);
	vec4 snowDetiled = vec4(blendRGB(snowUpper.rgb, snowMid.rgb, snowLower.rgb, 0.333), 1.0);

    vec4 cliffTextureLower = vec4(texture(cliffSampler, texCoords * 10).rgb * 0.7, 1.0);
	vec4 cliffTextureMid = vec4(texture(cliffSampler, texCoords * 20).rgb * 0.7, 1.0);
	vec4 cliffTextureUpper = vec4(texture(cliffSampler, texCoords * 30).rgb * 0.7, 1.0);
	vec4 cliffDetiled = vec4(blendRGB(cliffTextureLower.rgb, cliffTextureMid.rgb, cliffTextureUpper.rgb, 0.333), 1.0);


    vec4 sandTextureUpper = vec4(texture(sandSampler, texCoords * 20).rgb * 1.0, 1.0);
	vec4 sandTextureMid = vec4(texture(sandSampler, texCoords * 30 + 30).rgb * 1.0, 1.0);
	vec4 sandTextureLower = vec4(texture(sandSampler, texCoords * 40 + 40).rgb * 1.0, 1.0);
	vec4 sandDetiled = vec4(blendRGB(sandTextureUpper.rgb, sandTextureMid.rgb, sandTextureLower.rgb, 0.333), 1.0);

    vec3 matColor;
    matColor = mix(vec3(cliffDetiled), vec3(grassDetiled), smoothstep(0.6, 0.8, normal.y));
    matColor = mix(matColor, vec3(sandDetiled), smoothstep(0.9, 1.0, normal.y ));

    float isSnow = smoothstep(snowBreakpoint, snowBreakpoint + 0.1, height * (0.5 + 0.5 * normal.y));
    matColor = mix(matColor, vec3(snowDetiled), isSnow);

     // lighting
    vec4 eyepos_local = inverse(view * model) * vec4(0.0, 0.0, 0.0, 1.0);
    vec3 viewDir = normalize(eyepos_local.xyz - vertex);
    vec3 lightDir = normalize(sunPosition - vertex);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 n = normalize(normal);
    vec3 diffuse_wrapped = saturate((dot(n, lightDir) + diffuseAmplitude) / (1.0 + diffuseAmplitude)) * lightColor * matColor;
    //vec3 diffuse = max(dot(n, -sunPosition), 0.0) * lightColor * matColor * diffuseAmplitude;
    
    float shadow = calcShadow(vertex_light_clipspace, n, lightDir);
    vec3 finalColor = shadow > 0.0 ? ((1.0 - shadow) + shadowAlpha) * diffuse_wrapped : diffuse_wrapped;


    //  //fog
    // float dist = length(vec3(view * model * vec4(vertex, 1.0)));
    // vec3 vertex_worldspace = vec3(model * vec4(vertex, 1.0));
    // float smoothing = smoothstep(fogLowerBound, fogUpperBound, fogUpperBound - vertex_worldspace.y);
    // float de1 = de * smoothing;
    // float di1 = di * smoothing;
    // float extintion = exp(-dist * de1);
    // float inscattering = exp(-dist * di1);
    // finalColor = vec3(vec4(finalColor, 1.0) * extintion + vec4(fogColor, 1.0) * (1.0 - inscattering));

    //wireframe
    float distanceToEdge = min(edgeDistance[0], min(edgeDistance[1], edgeDistance[2]));
    if(toggleGrid && distanceToEdge < lineWidth)
        finalColor = vec3(0.0, 1.0, 0.0);

    return finalColor;
}

void main()
{
    if(showSSAO)
        fragColor = vec4(0.95, 0.95, 0.95, 1.0);
    else
        fragColor = vec4(shadeTerrain(In.vertex.xyz, In.vertex_viewspace.xyz, In.normal, In.texCoords, In.vertex_light_clipspace), 1.0);
    position = vec4(In.vertex.xyz, 1.0);
    normal = normalize(In.normal.xyz * transpose(inverse(mat3(view * model))));
    occlusionColor = vec4(0.0, 0.0, 0.0, 1.0);
}