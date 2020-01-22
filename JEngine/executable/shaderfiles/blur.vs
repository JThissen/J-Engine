#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    // TexCoords = aTexCoords;
     gl_Position = vec4(position, 1.0);
    //gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	TexCoords = texCoords;
}

