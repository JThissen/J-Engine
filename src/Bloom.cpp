#include "Bloom.h"
#include "Helpers.h"

Bloom::Bloom() {}

Bloom::Bloom(int windowWidth, int windowHeight)
{
	vs_blur = "./ShaderFiles/blur.vs";
	fs_blur = "./ShaderFiles/blur.fs";
	vs_bloom = "./ShaderFiles/bloom.vs";
	fs_bloom = "./ShaderFiles/bloom.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_blur)
		.AttachFragmentShader(fs_blur);
	shader_blur = shaderBuilder.BuildShader();

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_bloom)
		.AttachFragmentShader(fs_bloom);
	shader_bloom = shaderBuilder.BuildShader();

	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	quadVAO = 0;

	//1x fbo, 2x color buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	glGenTextures(1, &textureHdrBuffer);
	glBindTexture(GL_TEXTURE_2D, textureHdrBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureHdrBuffer, 0);

	glDrawBuffers(2, attachments);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//ping pong buffers (2x fbo + 2x color buffer)
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer is not complete!" << std::endl;
	}

	shader_blur.UseShader();
	glUniform1i(glGetUniformLocation(shader_blur.GetShaderProgram(), "textureHdrBuffer"), GL_TEXTURE0);

	shader_bloom.UseShader();
	glUniform1i(glGetUniformLocation(shader_bloom.GetShaderProgram(), "scene"), GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader_bloom.GetShaderProgram(), "bloomblur"), GL_TEXTURE1);
}

unsigned int Bloom::getFBO()
{
	return FBO;
}

unsigned int Bloom::gettextureColorBuffer()
{
	return textureColorBuffer;
}

unsigned int Bloom::gettextureHdrBuffer()
{
	return textureHdrBuffer;
}

void Bloom::blur(int amount)
{
	shader_blur.UseShader();
	horizontal = true;
	bool first_iteration = true;
	for (int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		glUniform1i(glGetUniformLocation(shader_blur.GetShaderProgram(), "horizontal"), horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? textureHdrBuffer : pingpongColorbuffers[!horizontal]);
		Helpers::Draw::drawQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::draw(float exposure)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_bloom.UseShader();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHdrBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	glUniform1f(glGetUniformLocation(shader_bloom.GetShaderProgram(), "exposure"), exposure);
	Helpers::Draw::drawQuad();
}
