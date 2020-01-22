#version 460 core

out vec4 FragColor;

in block
{
    vec2 TexCoords;
} In;

uniform sampler2D positionBuffer;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightPosition;

void main()
{
	// vec3 position_local = texture(positionBuffer, In.TexCoords).xyz;
	// vec3 position_clip = projection * view * model * vec4(position_local, 1.0);

	// FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragDepth = gl_FragCoord.z;
}