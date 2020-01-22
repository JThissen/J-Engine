#version 460 core

layout (location = 1) in block 
{
    vec3 vertex;
    vec3 vertex_viewspace;
    vec3 normal;
    vec2 texCoords;
} In;


layout (location = 2) out vec4 position;
layout (location = 1) out vec3 normal;
layout (location = 0) out vec4 fragColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 tileSize;
uniform int gridW;
uniform int gridH;
uniform float shininess;
uniform vec3 ambientColor;
uniform float wrap;
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
uniform float heightMin;
uniform float heightMax;
uniform bool showSSAO;
uniform float time;
uniform sampler2D waterNormals;
uniform sampler2D colorBuffer;
uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D vector2dmap;
uniform sampler2D refractionDepthBuffer;
uniform float distortionStrength;
uniform vec3 shallowColor;
uniform vec3 deepColor;
uniform float specularStrength;
uniform float mixStrength;


noperspective in vec3 edgeDistance;
const vec3 sunColor = vec3(1.0, 1.0, 0.7);
const vec3 lightColor = vec3(1.0, 1.0, 0.7)*1.5;
float near = 0.1; 
float far  = 100.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; //ndc space
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

vec4 shadeWater(vec3 vertex, vec3 vertex_viewspace, vec3 normal, vec2 texCoords)
{   
    //lighting, reflection, refraction, normal map and distortion map
    normal = normalize(normal * transpose(inverse(mat3(model))));
    vec3 vertex_worldspace = vec3(model * vec4(vertex, 1.0));
    vec3 camera_worldspace = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 sunPosition = vec3(10.0, 10.0, -10.0);
    vec3 lightDir = normalize(sunPosition - vertex_worldspace);
    vec3 viewDir = normalize(camera_worldspace - vertex_worldspace);

    vec3 waterN1 = vec4(texture(waterNormals, texCoords * 20 + vec2(time * 0.008, time * 0.016), 0).rgb * 1, 1.0).rgb;
    vec3 waterN2 = vec4(texture(waterNormals, texCoords * 10 + vec2(time * -0.011, time * -0.022), 0).rgb * 1, 1.0).rgb;
    vec3 norm = mix(waterN1, waterN2, 0.5);
    
	vec3 ambientFactor = vec3(0.0);
	vec3 diffuseFactor = vec3(1.0);
	
	if (dot(norm, viewDir) < 0) norm = -norm;
	
    vec3 ambient = ambientColor * ambientFactor;

	float relativeHeight;
	relativeHeight = (vertex_worldspace.y - heightMin) / (heightMax - heightMin);
	vec3 heightColor = relativeHeight * shallowColor + (1 - relativeHeight) * deepColor;

    vec3 halfwayDir = normalize(viewDir + lightDir);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * ambientColor * diff;
	
	float refCoeff = pow(max(dot(norm, viewDir), 0.0), 0.9);
	vec3 reflectColor = (1 - refCoeff) * shallowColor;
	vec3 reflectDir = reflect(-lightDir, norm); 

    float shine = 13.0;
	float specCoeff = pow(max(dot(norm, halfwayDir), 0.0), shine);
    vec3 specular = sunColor * specCoeff * specularStrength;

    vec4 vertex_clipspace = projection * view * model * vec4(vertex, 1.0);
    vec2 vertex_ndcspace = vertex_clipspace.xy / vertex_clipspace.w;
    vec2 vertex_ndc01 = vertex_ndcspace / 2.0 + 0.5;
    vec2 distortion1 = (texture(vector2dmap, vec2(texCoords.x * 30.0 + time * 0.1, texCoords.y * 30.0)).xy * 2.0 - 1.0) * 0.02;
    vec2 distortion2 = (texture(vector2dmap, vec2(-texCoords.x * 30.0 + time * 0.1, texCoords.y * 30.0 + time * 0.1)).xy * 2.0 - 1.0) * 0.02;
    vec2 totalDistortion = distortion1 + distortion2;
    vec2 reflectCoords = vec2(vertex_ndc01.x, -vertex_ndc01.y) + totalDistortion;
    reflectCoords.x = clamp(reflectCoords.x, 0.001, 0.999);
    reflectCoords.y = clamp(reflectCoords.y, -0.999, -0.001);
    vec2 refractCoords = clamp(vertex_ndc01 + totalDistortion, 0.001, 0.999);
    vec4 reflectionColor = texture(reflection, reflectCoords);
    vec4 refractionColor = texture(refraction, refractCoords);

    float fresnel = dot(viewDir, normal);
    vec3 color = mix(reflectionColor, refractionColor, fresnel).xyz;
	color += ambient + diffuse + heightColor + reflectColor + specular;

    //wireframe
    float distanceToEdge = min(edgeDistance[0], min(edgeDistance[1], edgeDistance[2]));
    if(toggleGrid && distanceToEdge < lineWidth)
        color = vec3(1.0, 1.0, 1.0);

    //blended edges
    float d = texture(refractionDepthBuffer, vertex_ndc01).r;
    float floorDistance = LinearizeDepth(d.r) / far;
    float waterDistance = LinearizeDepth(gl_FragCoord.z) / far;
    float waterDepth = (floorDistance - waterDistance);
    vec4 newDepth = vec4(vec3(waterDepth / mixStrength), 1.0);
    vec4 finalColor = vec4(color, clamp(newDepth.x / 0.1, 0.0, 1.0));
    return finalColor;
}

void main()
{
    if(showSSAO)
        fragColor = vec4(0.95, 0.95, 0.95, 1.0);
    else
        fragColor = shadeWater(In.vertex.xyz, In.vertex_viewspace.xyz, In.normal, In.texCoords);
    position = vec4(In.vertex.xyz, 1.0);
    normal = normalize(In.normal.xyz * transpose(inverse(mat3(view * model))));
}