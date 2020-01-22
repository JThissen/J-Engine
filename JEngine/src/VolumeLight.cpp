#include "VolumeLight.h"


VolumeLight::VolumeLight(int window_width, int window_height, int samples, float exposure,
	float decay, float density, float weight, float bounds, glm::vec3 light_position)
	:	window_width(window_width),window_height(window_height),samples(samples),
		exposure(exposure),decay(decay), density(density), weight(weight), bounds(bounds),
		light_position(light_position)
{
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	vs_volumelight = "./ShaderFiles/volumelight.vs";
	fs_volumelight = "./ShaderFiles/volumelight.fs";
	vs_occlusion = "./ShaderFiles/occlusion.vs";
	fs_occlusion = "./ShaderFiles/occlusion.fs";
	vs_normal = "./ShaderFiles/normal.vs";
	fs_normal = "./ShaderFiles/normal.fs";
	vs_light = "./ShaderFiles/light.vs";
	fs_light = "./ShaderFiles/light.fs";

	shader_builder.create_shader().attach_vertex_shader(vs_volumelight).attach_fragment_shader(fs_volumelight);
	shader_volumelight = shader_builder.build_shader();

	shader_builder.create_shader().attach_vertex_shader(vs_occlusion).attach_fragment_shader(fs_occlusion);
	shader_occlusion = shader_builder.build_shader();

	shader_builder.create_shader().attach_vertex_shader(vs_normal).attach_fragment_shader(fs_normal);
	shader_normal = shader_builder.build_shader();

	shader_builder.create_shader().attach_vertex_shader(vs_light).attach_fragment_shader(fs_light);
	shader_light = shader_builder.build_shader();

	glGenFramebuffers(2, frame_buffer);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer[i]);

		glGenTextures(1, &color_buffer[i]);
		glBindTexture(GL_TEXTURE_2D, color_buffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer[i], 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &depth_buffer[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer is incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader_volumelight.use_shader();
	glUniform1i(glGetUniformLocation(shader_volumelight.get_shader_program(), "tex1"), 0);
	glUniform1i(glGetUniformLocation(shader_volumelight.get_shader_program(), "tex2"), 1);
}

void VolumeLight::calc_movement(float delta_time, float velocity)
{
	distance = delta_time * velocity;

	if (light_position.x >= bounds)
		direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	else if (light_position.x <= -bounds)
		direction = glm::vec3(1.0f, 0.0f, 0.0f);

	light_position += distance * direction;
}

void VolumeLight::draw_occlusion(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer[0]);
	glBindTexture(GL_TEXTURE_2D, color_buffer[0]);
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_occlusion.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(view));

	for (int i = 0; i < 2; i++)
	{
		model = glm::mat4(1.0f);

		if (i == 0)
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
		else if (i == 1)
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		Helpers::Draw::draw_cube();
	}

	shader_light.use_shader();
	model = glm::mat4(1.0f);
	model = glm::translate(model, light_position);
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	Helpers::Draw::draw_cube();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumeLight::draw_normal(glm::mat4 m_projection, glm::mat4 m_view, glm::mat4 m_model)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer[1]);
	glBindTexture(GL_TEXTURE_2D, color_buffer[1]);
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_normal.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));

	for (int i = 0; i < 2; i++)
	{
		m_model = glm::mat4(1.0f);

		if (i == 0)
			m_model = glm::translate(m_model, glm::vec3(-1.0f, 0.0f, 1.0f));
		else if (i == 1)
			m_model = glm::translate(m_model, glm::vec3(2.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
		Helpers::Draw::draw_cube();
	}

	shader_light.use_shader();
	m_model = glm::mat4(1.0f);
	m_model = glm::translate(m_model, light_position);
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
	Helpers::Draw::draw_cube();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumeLight::draw_light_shafts(glm::mat4 m_projection, glm::mat4 m_view)
{
	shader_volumelight.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader_volumelight.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_volumelight.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));

	m_pv = m_projection * m_view;
	light_pos_ndc = m_pv * glm::vec4(light_position, 1);
	light_pos_ndc /= light_pos_ndc.w;
	light_pos_screen = glm::vec2((light_pos_ndc.x + 1) * 0.5, (light_pos_ndc.y + 1) * 0.5);

	glUniform2fv(glGetUniformLocation(shader_volumelight.get_shader_program(), "lightScreenPos"), 1, glm::value_ptr(light_pos_screen));
	glUniform1i(glGetUniformLocation(shader_volumelight.get_shader_program(), "samples"), samples);
	glUniform1f(glGetUniformLocation(shader_volumelight.get_shader_program(), "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader_volumelight.get_shader_program(), "decay"), decay);
	glUniform1f(glGetUniformLocation(shader_volumelight.get_shader_program(), "density"), density);
	glUniform1f(glGetUniformLocation(shader_volumelight.get_shader_program(), "weight"), weight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_volumelight.use_shader();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_buffer[0]);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, color_buffer[1]);
	Helpers::Draw::draw_quad();
}

unsigned int VolumeLight::get_color_buffer(int value) const { return color_buffer[value]; }
int VolumeLight::get_samples() const { return samples; }
float VolumeLight::get_exposure() const { return exposure; }
float VolumeLight::get_decay() const { return decay; }
float VolumeLight::get_density() const { return density; }
float VolumeLight::get_weight() const { return weight; }
