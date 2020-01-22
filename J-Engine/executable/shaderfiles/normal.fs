#version 330 core
//out vec4 textureColorBuffer;
uniform vec3 lightColor;;

// layout (location = 4) out vec4 textureColorBuffer_copy;
layout (location = 4) out vec4 textureColorBuffer;

void main()
{
	textureColorBuffer = vec4(lightColor, 1.0);   
	//vec4 textureColorBuffer_copy = textureColorBuffer;          
}