#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <random>
#include <memory>

#include "Shader.h"
#include "View.h"
#include "ShaderBuilder.h"
#include "Water.h"
#include "SSAO.h"

class Terrain
{
private:
	unsigned int		window_width;
	unsigned int		window_height;
	unsigned int		FBO;
	unsigned int		color_buffer;
	unsigned int		position_buffer;
	unsigned int		normal_buffer;
	unsigned int		depth_buffer;
	unsigned int		VAO;
	unsigned int		VBO;
	unsigned int		ssao_color_buffer_blur;
	unsigned int		reflection_FBO;
	unsigned int		refraction_FBO;
	unsigned int		refraction_depth_buffer;
	unsigned int		reflection_color_buffer;
	unsigned int		refraction_color_buffer;
	unsigned int		grass_texture;
	unsigned int		cliff_texture;
	unsigned int		snow_texture;
	unsigned int		sand_texture;
	unsigned int		height_map_texture;
	unsigned int		color_buffer_water;
	unsigned int		texture_cubemap;
	unsigned int		shadow_depth_FBO;
	unsigned int		shadow_depth_buffer;
	unsigned int		occlusion_buffer;
	int					inner_tessellation;
	int					outer_tessellation;
	int					inner_tessellation_water;
	int					outer_tessellation_water;
	int					refl_refr_tessellation;
	int					grid_w;
	int					grid_h;
	int					noise_octaves;
	int					noise_size;
	int					noise_texture;
	int					noise_texture_3d;
	int					instances;
	int					mode;
	int					shadow_samples;
	float				noise_frequency;
	float				height_scale;
	float				specular_shininess;
	float				wrapped_diffuse;
	float				fog_exp;
	float				de;
	float				di;
	float				fog_upper_bound;
	float				fog_lower_bound;
	float				line_width;
	float				min_depth;
	float				max_depth;
	float				min_tessellation;
	float				max_tessellation;
	float				time;
	float				water_height;
	float				specular_strength;
	float				mix_strength;
	float				snow_breakpoint;
	float				shadow_bias;
	float				shadow_alpha;
	bool				toggle_grid;
	bool				cull;
	bool				camera_lod;
	bool				show_ssao;
	ShaderBuilder		shader_builder;
	Shader				shader_terrain;
	Shader				shader_lighting;
	Shader				shader_water;
	Shader				shader_terrain_empty;
	Shader				shader1_terrain;
	Shader				shader_normal;
	Water				water;
	SSAO				ssao;
	glm::vec2			translate;
	glm::vec3			tile_size;
	glm::vec3			ambient_color;
	glm::vec3			light_color;
	glm::vec3			fog_color;
	glm::vec3			sun_position;
	glm::vec3			shallow_color;
	glm::vec3			deep_color;
	glm::vec3			sky_color;
	glm::vec3			light_pos;
	glm::mat4			light_projection;
	glm::mat4			light_view;
	std::vector<float>	vertex_data;
	std::vector<short>	index_data;

	int samples;
	float exposure;
	float decay;
	float density;
	float weight;

	glm::vec3 volume_light_color;

public:
	Terrain() = default;
	Terrain(int window_width, int window_height);
	void initialize_parameters();
	void create_shaders();
	void create_buffers();
	void draw(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view);
	unsigned int load_terrain_texture(std::string path);
	void render_terrain(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view, Shader& shader, 
		unsigned int fbo, bool reflection_refraction, bool use_empty);
	void render_post_processing(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model);
	void render_sun(const glm::mat4& m_projection, const glm::mat4& m_view);
};

#endif