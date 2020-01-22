#include "SSAO.h"

SSAO::SSAO(int window_width, int window_height) : window_width(window_width), window_height(window_height)
{
	initialize_shader_parameters();
	create_SSAO_shader();
	create_SSAO_blur_shader();
	create_buffers();
}

void SSAO::initialize_shader_parameters()
{
	SSAO_samples = 64;
	SSAO_noise_texture_width = 4;
	kernel_size = 64;
	radius = 0.611f;
	bias = 0.012f;
	blur_SSAO = true;
	show_SSAO = false;
}

void SSAO::create_SSAO_shader()
{
	std::string vs_ssao = "./ShaderFiles/ssao.vs";
	std::string fs_ssao = "./ShaderFiles/ssao.fs";

	shader_builder.create_shader().attach_vertex_shader(vs_ssao).attach_fragment_shader(fs_ssao);
	shader_SSAO = shader_builder.build_shader();
	shader_SSAO.use_shader();

	glUniform1i(glGetUniformLocation(shader_SSAO.get_shader_program(), "colorBuffer"), 0);
	glUniform1i(glGetUniformLocation(shader_SSAO.get_shader_program(), "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shader_SSAO.get_shader_program(), "gPosition"), 2);
	glUniform1i(glGetUniformLocation(shader_SSAO.get_shader_program(), "texNoise"), 3);

	std::uniform_real_distribution<GLfloat> random_floats(0.0f, 1.0f);
	std::default_random_engine generator;
	std::vector<glm::vec3> SSAO_noise;

	for (int i = 0; i < SSAO_samples; i++)
	{
		glm::vec3 sample(random_floats(generator) * 2.0f - 1.0f, random_floats(generator) * 2.0f - 1.0f, random_floats(generator));
		sample = glm::normalize(sample);
		sample *= random_floats(generator);

		float scale = static_cast<float>(i) / static_cast<float>(SSAO_samples); //closer to origin (x^2)
		sample *= Helpers::Utility::lerp<float>(0.1f, 1.0f, scale * scale);
		std::string item = "samples[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(shader_SSAO.get_shader_program(), item.c_str()), 1, glm::value_ptr(sample));
	}

	for (int i = 0; i < (SSAO_noise_texture_width * SSAO_noise_texture_width); i++)
	{
		glm::vec3 noise(random_floats(generator) * 2.0f - 1.0f, random_floats(generator) * 2.0f - 1.0f, 0.0f);
		SSAO_noise.push_back(noise);
	}

	glGenTextures(1, &SSAO_noise_texture);
	glBindTexture(GL_TEXTURE_2D, SSAO_noise_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SSAO_noise_texture_width, SSAO_noise_texture_width, 0, GL_RGB, GL_FLOAT, &SSAO_noise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::create_SSAO_blur_shader()
{
	std::string vs_blur = "./ShaderFiles/blur2.vs";
	std::string fs_blur = "./ShaderFiles/blur2.fs";

	shader_builder.create_shader().attach_vertex_shader(vs_blur).attach_fragment_shader(fs_blur);
	shader_SSAO_blur = shader_builder.build_shader();
	shader_SSAO_blur.use_shader();
	glUniform1i(glGetUniformLocation(shader_SSAO_blur.get_shader_program(), "ssaoInput"), 0);
}

void SSAO::create_buffers()
{
	glGenFramebuffers(1, &SSAO_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_FBO);
	glGenTextures(1, &SSAO_color_buffer);
	glBindTexture(GL_TEXTURE_2D, SSAO_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAO_color_buffer, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ssaFBO is incomplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &SSAO_blur_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_blur_FBO);
	glGenTextures(1, &SSAO_blur_color_buffer);
	glBindTexture(GL_TEXTURE_2D, SSAO_blur_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAO_blur_color_buffer, 0);
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ssaoBlurFBO is incomplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SSAO::render_SSAO(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, unsigned int color_buffer,
	unsigned int normal_buffer, unsigned int position_buffer, unsigned int& ssao_color_buffer_blur, bool& display_ssao)
{
	Helpers::Gui::imgui_ssao(window_width, window_height, kernel_size, radius, bias, blur_SSAO, show_SSAO);
	display_ssao = show_SSAO;

	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_FBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shader_SSAO.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader_SSAO.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_SSAO.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_SSAO.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
	glUniform1i(glGetUniformLocation(shader_SSAO.get_shader_program(), "kernelSize"), kernel_size);
	glUniform1f(glGetUniformLocation(shader_SSAO.get_shader_program(), "radius"), radius);
	glUniform1f(glGetUniformLocation(shader_SSAO.get_shader_program(), "bias"), bias);
	glUniform1f(glGetUniformLocation(shader_SSAO.get_shader_program(), "windowWidth"), static_cast<float>(window_width));
	glUniform1f(glGetUniformLocation(shader_SSAO.get_shader_program(), "windowHeight"), static_cast<float>(window_height));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normal_buffer);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, position_buffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, SSAO_noise_texture);
	Helpers::Draw::draw_quad();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_blur_FBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shader_SSAO_blur.use_shader();
	glUniform1i(glGetUniformLocation(shader_SSAO_blur.get_shader_program(), "blurSSAO"), blur_SSAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSAO_color_buffer);
	Helpers::Draw::draw_quad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ssao_color_buffer_blur = SSAO_blur_color_buffer;
}