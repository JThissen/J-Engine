#version 460 core
out vec4 FragColor;

in position_block
{
    vec4 pos_worldspace;
    vec4 pos_viewspace;
} In;

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D ssao;
uniform sampler2D depthBuffer;
uniform sampler2D occlusionBuffer;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float de;
uniform float di;
uniform float fogHeight;
uniform float fogUpperBound;
uniform float fogLowerBound;
uniform vec3 fogColor;
uniform vec3 ambientColor;
uniform bool showSSAO;
uniform vec3 sunPosition;
uniform vec3 lightPosition;
uniform vec3 skyColor;
uniform mat4 lightProjection;
uniform mat4 lightView;
uniform int samples;
uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;

const vec3 sunColor = vec3(1.0, 1.0, 0.3);
const vec3 backgroundColor = vec3(0.7, 0.7, 0.7);
const float near = 0.1; 
const float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; //ndc space
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec4 sky(vec3 origin, vec3 direction)
{
    vec3 sky = mix(skyColor, backgroundColor, pow(min(1.0, 0.75-direction.y), 10.0));
    return vec4(sky, 1.0);
}

float calcShadow(vec4 vertex_light_clipspace)
{
    vec3 vertex_light_ndcspace = vertex_light_clipspace.xyz / vertex_light_clipspace.w;
    vec3 vertex_light_depthspace = vertex_light_ndcspace * 0.5 + 0.5; // to [0 : 1]
    float shadowbuffer_depth = texture(depthBuffer, vertex_light_depthspace.xy).r;
    float vertex_depth = vertex_light_depthspace.z;
    float isShadow = vertex_depth  > shadowbuffer_depth ? 1.0 : 0.0;
    return isShadow;
}

vec4 calcFog(vec4 finalColor, vec3 pos)
{
    float dist = length(vec3(view * model * vec4(pos, 1.0)));
    vec3 vertex_worldspace = vec3(model * vec4(pos, 1.0));
    float smoothing = smoothstep(fogLowerBound, fogUpperBound, fogUpperBound - vertex_worldspace.y);
    float de1 = de * smoothing;
    float di1 = di * smoothing;
    float extintion = exp(-dist * de1);
    float inscattering = exp(-dist * di1);
    return finalColor = finalColor * extintion + vec4(fogColor, 1.0) * (1.0 - inscattering);
}

vec4 calcVolumetricLight()
{
    vec4 lightPosition_vec4 = projection * view * model * vec4(lightPosition, 1.0);
    lightPosition_vec4 /= lightPosition_vec4.w;
    vec2 lightPosition_vec2 = (vec2(lightPosition_vec4.x, lightPosition_vec4.y) + 1) * 0.5; //[0:1]
    vec4 outColor = vec4(0.0);
	vec2 textCoo = TexCoords.xy;
	vec2 deltaTextCoord = textCoo - lightPosition_vec2;
 	deltaTextCoord *= 1.0 /  float(samples) * density;
	float illuminationDecay = 1.0;

	for(int i=0; i < samples ; i++)
  	{
		textCoo -= deltaTextCoord;
		vec4 colorSample = vec4(texture(occlusionBuffer, textCoo).rgb, 1.0);
		colorSample  *= illuminationDecay * weight;
		outColor += colorSample;
		illuminationDecay *= decay;
 	}

    outColor *= exposure;
    outColor *= 0.5;
    return outColor;
}

void main()
{
    vec3 pos = texture(gPosition, TexCoords).rgb;
    vec4 color = texture(gAlbedo, TexCoords).rgba;
    float occlusion = texture(ssao, TexCoords).r;
    float depth = texture(depthBuffer, TexCoords).r;
    float d = LinearizeDepth(depth.r) / far;
    vec3 camera_worldspace = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 dir = normalize(In.pos_worldspace.xyz - camera_worldspace);
    vec4 vertex_light_clipspace = lightProjection * lightView * model * vec4(pos, 1);
    vec4 ambient = vec4(ambientColor, 1.0) * color * occlusion;
    vec4 finalColor = ambient;

    if(d < 0.9)
        finalColor = calcFog(finalColor, pos);

    finalColor += calcVolumetricLight();

    if(showSSAO)
        FragColor = color * occlusion;
    else
        FragColor = finalColor;
}