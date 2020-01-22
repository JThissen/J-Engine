#ifndef SSAO_H
#define SSAO_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <string>
#include <random>
#include <vector>

#include "Helpers.h"
#include "ShaderBuilder.h"

class SSAO
{
private:
	ShaderBuilder		shader_builder;
	Shader				shader_SSAO_blur;
	Shader				shader_SSAO;
	int					SSAO_samples;
	int					SSAO_noise_texture_width;
	int					window_width;
	int					window_height;
	int					kernel_size;
	unsigned int		SSAO_noise_texture;
	unsigned int		SSAO_FBO;
	unsigned int		SSAO_color_buffer;
	unsigned int		SSAO_blur_FBO;
	unsigned int		SSAO_blur_color_buffer;
	float				radius;
	float				bias;
	bool				blur_SSAO;
	bool				show_SSAO;

public:
	SSAO() = default;
	SSAO(int window_width, int window_height);

	void initialize_shader_parameters();
	void create_SSAO_shader();
	void create_SSAO_blur_shader();
	void create_buffers();
	void render_SSAO(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, unsigned int color_buffer,
		unsigned int normal_buffer, unsigned int position_buffer, unsigned int& ssao_color_buffer_blur, bool& display_ssao);
};

#endif