#ifndef VOLUMELIGHT_H
#define VOLUMELIGHT_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <string>

#include "Helpers.h"
#include "ShaderBuilder.h"

class VolumeLight
{
private:
	unsigned int	color_buffer[2];
	unsigned int	depth_buffer[2];
	unsigned int	frame_buffer[2];
	unsigned int	quad_VAO;
	unsigned int	quad_VBO;
	int				window_width;
	int				window_height;
	int				samples;
	float			exposure;
	float			decay;
	float			density;
	float			weight;
	float			bounds;
	float			distance;
	glm::vec3		direction;
	glm::vec3		light_position;
	glm::vec2		light_pos_screen;
	glm::vec4		light_pos_ndc;
	glm::mat4		m_pv;
	std::string		vs_volumelight;
	std::string		fs_volumelight;
	std::string		vs_occlusion;
	std::string		fs_occlusion;
	std::string		vs_normal;
	std::string		fs_normal;
	std::string		vs_light;
	std::string		fs_light;
	ShaderBuilder	shader_builder;
	Shader			shader_volumelight;
	Shader			shader_occlusion;
	Shader			shader_normal;
	Shader			shader_light;
	
public:
	VolumeLight() = default;
	VolumeLight(int window_width, int window_height, int samples = 100, float exposure = 0.038f, float decay = 1.0f, 
		float density = 0.962f, float weight = 0.476f, float bounds = 5.0f, glm::vec3 light_position = glm::vec3(-1.0f, 0.0f, -4.0f));
	void draw_occlusion(glm::mat4 m_projection, glm::mat4 m_view, glm::mat4 m_model);
	void draw_normal(glm::mat4 m_projection, glm::mat4 m_view, glm::mat4 m_model);
	void draw_light_shafts(glm::mat4 m_projection, glm::mat4 m_view);
	void calc_movement(float delta_time, float velocity = 4.0f);

	unsigned int get_color_buffer(int value) const;
	int get_samples() const;
	float get_exposure() const;
	float get_decay() const;
	float get_density() const;
	float get_weight() const;
};

#endif