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



// uniform sampler2D positionBuffer;


// uniform vec3 lightPosition;

// void main()
// {
// 	vec3 position_local = texture(positionBuffer, In.TexCoords).xyz;
// 	vec3 position_clip = projection * view * model * vec4(position_local, 1.0);

// 	FragColor = vec4(1.0, 0.0, 0.0, 1.0);
// }