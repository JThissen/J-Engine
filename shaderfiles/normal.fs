#version 460 core

uniform vec3 lightColor;;

layout (location = 4) out vec4 textureColorBuffer;

void main()
{
	textureColorBuffer = vec4(lightColor, 1.0);      
}