#include "Water.h"

Water::Water(int window_width, int window_height) : window_width(window_width), window_height(window_height)
{
	initialize_shader_parameters();
	create_shader();
	create_buffers();
	load_static_textures();
}

void Water::initialize_shader_parameters()
{
	vertex_data = { 0.0f, 0.0f, 0.0f, 1.0f };
	height_min = -0.157f;
	height_max = 0.392f;
	wavelength1 = 0.196f;
	wavelength2 = 0.118f;
	wavelength3 = 0.157f;
	wavelength4 = 0.196f;
	amplitude1 = 0.004f;
	amplitude2 = 0.010f;
	amplitude3 = 0.009f;
	amplitude4 = 0.009f;
	speed1 = -0.039f;
	speed2 = -0.039f;
	speed3 = 0.039f;
	speed4 = 0.039f;
	specular_strength = 0.2f;
	mix_strength = 0.018f;
	dir1 = glm::vec2(0.508, 0.529);
	dir2 = glm::vec2(0.729, -0.395);
	dir3 = glm::vec2(1.0, 0.765);
	dir4 = glm::vec2(-0.288, 1.0);
	shallow_color = glm::vec3(0.0f, 0.64f, 0.68f);
	deep_color = glm::vec3(0.02f, 0.05f, 0.10f);
	toggle_grid = false;
	line_width = 0.002f;
}

void Water::create_shader()
{
	std::string vs_water = "./ShaderFiles/water.vs";
	std::string tesc_water = "./ShaderFiles/water.tesc";
	std::string tese_water = "./ShaderFiles/water.tese";
	std::string gs_water = "./ShaderFiles/water.gs";
	std::string fs_water = "./ShaderFiles/water.fs";

	shader_builder.create_shader()
		.attach_vertex_shader(vs_water)
		.attach_tess_control_shader(tesc_water)
		.attach_tess_eval_shader(tese_water)
		.attach_geometry_shader(gs_water)
		.attach_fragment_shader(fs_water);

	shader_water = shader_builder.build_shader();
	shader_water.use_shader();

	glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "waterNormals"), 0);
	glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "reflection"), 1);
	glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "refraction"), 2);
	glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "vector2dmap"), 3);
	glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "refractionDepthBuffer"), 4);
}

void Water::create_buffers()
{
	glGenVertexArrays(1, &VAO_water);
	glBindVertexArray(VAO_water);
	glGenBuffers(1, &VBO_water);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_water);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), &vertex_data.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(0);
}

void Water::load_static_textures()
{
	texture_water_normals = Helpers::Load::load_texture("./Images/textures/waterNormals.jpg");
	texture_vector_2dmap = Helpers::Load::load_texture("./Images/textures/2dvectormap.png");
}

void Water::render_water(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, unsigned int FBO,
	const glm::vec3& tile_size, int grid_w, int grid_h, int outer_tessellation, int inner_tessellation, const glm::vec3& sun_position,
	float specular_shininess, const glm::vec3& ambient_color, float time, unsigned int& reflection_color_buffer,
	unsigned int& refraction_color_buffer, unsigned int& refraction_depth_buffer)
{
	Helpers::Gui::imgui_water(window_width, window_height, height_min, height_max, dir1, dir2, dir3, dir4, wavelength1, wavelength2, wavelength3, wavelength4, amplitude1,
		amplitude2, amplitude3, amplitude4, speed1, speed2, speed3, speed4, inner_tessellation, outer_tessellation,
		specular_strength, mix_strength, shallow_color, deep_color, toggle_grid, line_width);

	texture_reflection_color_buffer = reflection_color_buffer;
	texture_refraction_color_buffer = refraction_color_buffer;
	texture_refraction_depth_buffer = refraction_depth_buffer;
	shader_water.use_shader();

	{
		glUniformMatrix4fv(glGetUniformLocation(shader_water.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniformMatrix4fv(glGetUniformLocation(shader_water.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
		glUniformMatrix4fv(glGetUniformLocation(shader_water.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "toggleGrid"), toggle_grid);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "lineWidth"), line_width);
		glUniform3fv(glGetUniformLocation(shader_water.get_shader_program(), "tileSize"), 1, glm::value_ptr(tile_size));
		glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "gridW"), grid_w);
		glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "gridH"), grid_h);
		glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "outerTessellationWater"), outer_tessellation);
		glUniform1i(glGetUniformLocation(shader_water.get_shader_program(), "innerTessellationWater"), inner_tessellation);
		glUniform3fv(glGetUniformLocation(shader_water.get_shader_program(), "sunPosition"), 1, glm::value_ptr(sun_position));
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "shininess"), specular_shininess);
		glUniform3fv(glGetUniformLocation(shader_water.get_shader_program(), "ambientColor"), 1, glm::value_ptr(ambient_color));
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "time"), time);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "heightMin"), height_min);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "heightMax"), height_max);
		glUniform2fv(glGetUniformLocation(shader_water.get_shader_program(), "dir1"), 1, glm::value_ptr(dir1));
		glUniform2fv(glGetUniformLocation(shader_water.get_shader_program(), "dir2"), 1, glm::value_ptr(dir2));
		glUniform2fv(glGetUniformLocation(shader_water.get_shader_program(), "dir3"), 1, glm::value_ptr(dir3));
		glUniform2fv(glGetUniformLocation(shader_water.get_shader_program(), "dir4"), 1, glm::value_ptr(dir4));
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "wavelength1"), wavelength1);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "wavelength2"), wavelength2);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "wavelength3"), wavelength3);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "wavelength4"), wavelength4);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "amplitude1"), amplitude1);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "amplitude2"), amplitude2);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "amplitude3"), amplitude3);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "amplitude4"), amplitude4);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "speed1"), speed1);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "speed2"), speed2);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "speed3"), speed3);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "speed4"), speed4);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "specularStrength"), specular_strength);
		glUniform1f(glGetUniformLocation(shader_water.get_shader_program(), "mixStrength"), mix_strength);
		glUniform3fv(glGetUniformLocation(shader_water.get_shader_program(), "shallowColor"), 1, glm::value_ptr(shallow_color));
		glUniform3fv(glGetUniformLocation(shader_water.get_shader_program(), "deepColor"), 1, glm::value_ptr(deep_color));
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture_water_normals);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texture_reflection_color_buffer);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texture_refraction_color_buffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, texture_vector_2dmap);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, texture_refraction_depth_buffer);
	glBindVertexArray(VAO_water);
	glDrawArraysInstanced(GL_PATCHES, 0, 1, grid_w * grid_h);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}