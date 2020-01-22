#ifndef WATER_H
#define WATER

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <string>
#include <vector>

#include "Helpers.h"
#include "ShaderBuilder.h"

class Water
{
private:
	ShaderBuilder			shader_builder;
	Shader					shader_water;
	unsigned int			VAO_water;
	unsigned int			VBO_water;
	unsigned int			texture_water_normals;
	unsigned int			texture_vector_2dmap;
	unsigned int			texture_reflection_color_buffer;
	unsigned int			texture_refraction_color_buffer;
	unsigned int			texture_refraction_depth_buffer;
	int						window_width;
	int						window_height;
	float					height_min;
	float					height_max;
	float					wavelength1;
	float					wavelength2;
	float					wavelength3;
	float					wavelength4;
	float					amplitude1;
	float					amplitude2;
	float					amplitude3;
	float					amplitude4;
	float					speed1;
	float					speed2;
	float					speed3;
	float					speed4;
	float					specular_strength;
	float					mix_strength;
	float					line_width;
	bool					toggle_grid;
	glm::vec2				dir1;
	glm::vec2				dir2;
	glm::vec2				dir3;
	glm::vec2				dir4;
	glm::vec3				shallow_color;
	glm::vec3				deep_color;
	std::vector<float>		vertex_data;

public:
	Water() = default;
	Water(int window_width, int window_height);
	void initialize_shader_parameters();
	void create_shader();
	void create_buffers();
	void load_static_textures();
	void render_water(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, unsigned int FBO, const glm::vec3& tile_size, 
		int grid_w, int grid_h, int outer_tessellation, int inner_tessellation, const glm::vec3& sun_position, float specular_shininess, 
		const glm::vec3& ambient_color, float time, unsigned int& reflection_color_buffer, unsigned int& refraction_color_buffer, unsigned int& refraction_depth_buffer);
};

#endif