#include "Bloom.h"

Bloom::Bloom(int window_width, int window_height)
{
	vs_blur = "./ShaderFiles/blur.vs";
	fs_blur = "./ShaderFiles/blur.fs";
	vs_bloom = "./ShaderFiles/bloom.vs";
	fs_bloom = "./ShaderFiles/bloom.fs";

	shader_builder.create_shader()
		.attach_vertex_shader(vs_blur)
		.attach_fragment_shader(fs_blur);
	shader_blur = shader_builder.build_shader();

	shader_builder.create_shader()
		.attach_vertex_shader(vs_bloom)
		.attach_fragment_shader(fs_bloom);
	shader_bloom = shader_builder.build_shader();

	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	quad_VAO = 0;

	//1x fbo, 2x color buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &texture_color_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

	glGenTextures(1, &texture_hdr_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_hdr_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_hdr_buffer, 0);

	glDrawBuffers(2, attachments);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//ping pong buffers (2x fbo + 2x color buffer)
	glGenFramebuffers(2, pingpong_FBO);
	glGenTextures(2, pingpong_color_buffer);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpong_FBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpong_color_buffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpong_color_buffer[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer is not complete!" << std::endl;
	}

	shader_blur.use_shader();
	glUniform1i(glGetUniformLocation(shader_blur.get_shader_program(), "textureHdrBuffer"), GL_TEXTURE0);

	shader_bloom.use_shader();
	glUniform1i(glGetUniformLocation(shader_bloom.get_shader_program(), "scene"), GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader_bloom.get_shader_program(), "bloomblur"), GL_TEXTURE1);
}


void Bloom::blur(int amount)
{
	shader_blur.use_shader();
	horizontal = true;
	bool first_iteration = true;
	for (int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpong_FBO[horizontal]);
		glUniform1i(glGetUniformLocation(shader_blur.get_shader_program(), "horizontal"), horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? texture_hdr_buffer : pingpong_color_buffer[!horizontal]);
		Helpers::Draw::draw_quad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::draw(float exposure)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_bloom.use_shader();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_hdr_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, pingpong_color_buffer[!horizontal]);
	glUniform1f(glGetUniformLocation(shader_bloom.get_shader_program(), "exposure"), exposure);
	Helpers::Draw::draw_quad();
}

unsigned int Bloom::get_fbo() const { return FBO; }
unsigned int Bloom::get_texture_color_buffer() const { return texture_color_buffer; }
unsigned int Bloom::get_texture_hdr_buffer() const { return texture_hdr_buffer; }