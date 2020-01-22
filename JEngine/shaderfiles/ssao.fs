#version 460 core

out float FragColor;

in vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D colorBuffer;
uniform sampler2D texNoise;
uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform float windowWidth;
uniform float windowHeight;
uniform vec3 samples[64];

const vec2 noiseScale = vec2(windowWidth/4.0, windowHeight/4.0); 

void main()
{
    vec3 fragPos = vec3(view * model * vec4(texture(gPosition, TexCoords).xyz, 1.0));
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
   
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
  
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i]; //--sample to view space
        samplePos = fragPos + samplePos * radius; 
        
        //sample to screen space
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; //0.0 - 1.0
        
        float sampleDepth = (view * model * texture(gPosition, offset.xy)).z; // get terrain depth at sample position
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}