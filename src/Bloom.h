#ifndef BLOOM_H
#define BLOOM_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <string>

#include "Helpers.h"
#include "ShaderBuilder.h"

class Bloom
{
private:
	unsigned int	FBO;
	unsigned int	texture_color_buffer;
	unsigned int	texture_hdr_buffer;
	unsigned int	attachments[2];
	unsigned int	pingpong_FBO[2];
	unsigned int	pingpong_color_buffer[2];
	unsigned int	quad_VAO;
	unsigned int	quad_VBO;
	bool			horizontal;
	std::string		vs_blur;
	std::string		fs_blur;
	std::string		vs_bloom;
	std::string		fs_bloom;
	ShaderBuilder	shader_builder;
	Shader			shader_blur;
	Shader			shader_bloom;

public:
	Bloom() = default;
	Bloom(int window_width, int window_height);
	void blur(int amount);
	void draw(float exposure);

	unsigned int get_fbo() const;
	unsigned int get_texture_color_buffer() const;
	unsigned int get_texture_hdr_buffer() const;
};

#endif