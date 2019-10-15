#ifndef BLOOM_H
#define BLOOM_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <string>
#include "ShaderBuilder.h"

class Bloom
{
private:
	unsigned int	FBO;
	unsigned int	textureColorBuffer;
	unsigned int	textureHdrBuffer;
	unsigned int	attachments[2];
	unsigned int	pingpongFBO[2];
	unsigned int	pingpongColorbuffers[2];
	bool			horizontal;
	unsigned int	quadVAO;
	unsigned int	quadVBO;
	std::string		vs_blur;
	std::string		fs_blur;
	std::string		vs_bloom;
	std::string		fs_bloom;
	ShaderBuilder	shaderBuilder;
	Shader			shader_blur;
	Shader			shader_bloom;

public:
	Bloom();
	Bloom(int windowWidth, int windowHeight);
	unsigned int getFBO();
	unsigned int gettextureColorBuffer();
	unsigned int gettextureHdrBuffer();
	void blur(int amount);
	void draw(float exposure);
};


#endif