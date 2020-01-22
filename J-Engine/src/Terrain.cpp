#include "Terrain.h"
#include "Helpers.h"
#include "stb_image.h"
#include "Model.h"
#include "Mesh.h"

Terrain::Terrain(int window_width, int window_height) 
	: window_width(window_width), window_height(window_height)
{
	initialize_parameters();
	create_shaders();
	create_buffers();
}

void Terrain::initialize_parameters()
{
	water = Water(window_width, window_height);
	ssao = SSAO(window_width, window_height);
	vertex_data = { 0.0f, 0.0f, 0.0f, 1.0f };
	inner_tessellation = outer_tessellation = 32;
	inner_tessellation_water = outer_tessellation_water = 10;
	grid_w = grid_h = 16;
	instances = grid_w * grid_h;
	noise_frequency = 0.25f;
	noise_octaves = 8;
	translate = glm::vec2(0.0f, 0.0f);
	height_scale = 3.5f;
	noise_size = 300;
	tile_size = glm::vec3(1.0f, 0.0f, 1.0f);
	specular_shininess = 100.0f;
	ambient_color = glm::vec3(1.0f, 1.0f, 1.0f);
	light_color = glm::vec3(1.5f, 1.5f, 1.5f);
	wrapped_diffuse = 0.9f;
	fog_exp = 0.1f;
	de = 0.068f;
	di = 0.043f;
	fog_upper_bound = 1.668f;
	fog_lower_bound = 0.4f;
	fog_color = glm::vec3(0.9f, 0.9f, 0.9f);
	line_width = 0.001f;
	toggle_grid = false;
	cull = true;
	camera_lod = false;
	min_depth = 0.0f;
	max_depth = 25.0f;
	min_tessellation = 12.0f;
	max_tessellation = 32.0f;
	sun_position = glm::normalize(glm::vec3(-1.0, -0.25, 1.0));
	water_height = 0.1f;
	refl_refr_tessellation = 5;
	snow_breakpoint = 1.0f;
	sky_color = glm::vec3(0.7f, 0.8f, 1.0f) * 0.7f;
	light_pos = glm::vec3(43.0f, 11.0f, -31.0f);
	shadow_bias = 0.001f;
	shadow_alpha = 0.15f;
	shadow_samples = 7;
	samples = 100;
	exposure = 0.038f;
	decay = 1.0f;
	density = 0.962f;
	weight = 0.476f;
	volume_light_color = glm::vec3(1.0, 1.0, 0.8);
}

void Terrain::create_shaders()
{
	std::string vs_terrain = "./ShaderFiles/terrain.vs";
	std::string tesc_terrain = "./ShaderFiles/terrain.tesc";
	std::string tese_terrain = "./ShaderFiles/terrain.tese";
	std::string gs_terrain = "./ShaderFiles/terrain.gs";
	std::string fs_terrain = "./ShaderFiles/terrain.fs";
	std::string fs_terrainEmpty = "./ShaderFiles/terrainEmpty.fs";
	std::string vs_shadowDepth = "./ShaderFiles/shadowDepth.vs";
	std::string fs_shadowDepth = "./ShaderFiles/shadowDepth.fs";
	std::string vs_lighting = "./ShaderFiles/lighting.vs";
	std::string fs_lighting = "./ShaderFiles/lighting.fs";
	std::string vs_normal = "./ShaderFiles/normal.vs";
	std::string fs_normal = "./ShaderFiles/normal.fs";

	shader_builder.create_shader()
		.attach_vertex_shader(vs_normal)
		.attach_fragment_shader(fs_normal);

	shader_normal = shader_builder.build_shader();

	shader_builder.create_shader()
		.attach_vertex_shader(vs_terrain)
		.attach_tess_control_shader(tesc_terrain)
		.attach_tess_eval_shader(tese_terrain)
		.attach_geometry_shader(gs_terrain)
		.attach_fragment_shader(fs_terrain);

	shader_terrain = shader_builder.build_shader();
	shader_terrain.use_shader();
	noise_texture = Helpers::Noise::create_noise_texture_2d<300>(GL_R16F, true);
	grass_texture = Helpers::Load::load_texture("./Images/textures/grass.jpg");
	snow_texture = Helpers::Load::load_texture("./Images/textures/snow.jpg");
	sand_texture = Helpers::Load::load_texture("./Images/textures/brownish.jpg");
	cliff_texture = Helpers::Load::load_texture("./Images/textures/cliff.jpg");
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "randTex"), 0);
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "grassSampler"), 1);
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "snowSampler"), 2);
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "sandSampler"), 3);
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "cliffSampler"), 4);
	glUniform1i(glGetUniformLocation(shader_terrain.get_shader_program(), "shadowDepthBuffer"), 5);

	shader_builder.create_shader()
		.attach_vertex_shader(vs_terrain)
		.attach_tess_control_shader(tesc_terrain)
		.attach_tess_eval_shader(tese_terrain)
		.attach_geometry_shader(gs_terrain)
		.attach_fragment_shader(fs_terrainEmpty);

	shader_terrain_empty = shader_builder.build_shader();
	shader_terrain_empty.use_shader();
	glUniform1i(glGetUniformLocation(shader_terrain_empty.get_shader_program(), "randTex"), 0);

	shader_builder.create_shader()
		.attach_vertex_shader(vs_lighting)
		.attach_fragment_shader(fs_lighting);

	shader_lighting = shader_builder.build_shader();
	shader_lighting.use_shader();
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "gAlbedo"), 0);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "ssao"), 1);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "depthBuffer"), 2);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "gPosition"), 3);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "occlusionBuffer"), 4);
}

void Terrain::draw(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view)
{
	Helpers::Gui::imgui_tessellation_window(window_width, window_height, grid_w, grid_h, tile_size, outer_tessellation, inner_tessellation, noise_frequency,
		noise_octaves, translate, height_scale, noise_size, specular_shininess, ambient_color, wrapped_diffuse, fog_exp,
		de, di, fog_upper_bound, fog_lower_bound, fog_color, toggle_grid, line_width, cull, camera_lod, min_depth, max_depth,
		min_tessellation, max_tessellation, light_color, sun_position, snow_breakpoint, light_pos, shadow_bias, shadow_alpha, 
		shadow_samples, samples, exposure, decay, density, weight, volume_light_color);

	instances = grid_w * grid_h;
	time = static_cast<float>(glfwGetTime()) * 0.5f;
	light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	light_view = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	render_terrain(light_projection, light_view, m_model, view, shader_terrain_empty, shadow_depth_FBO, false, true);
	render_terrain(m_projection, m_view, m_model, view, shader_terrain, FBO, true, false);

	water.render_water(m_projection, m_view, m_model, FBO, tile_size, grid_w, grid_h, inner_tessellation_water, outer_tessellation_water,
		sun_position, specular_shininess, ambient_color, time, reflection_color_buffer, refraction_color_buffer, refraction_depth_buffer);
	
	ssao.render_SSAO(m_projection, m_view, m_model, color_buffer, normal_buffer, position_buffer, ssao_color_buffer_blur, show_ssao);

	render_sun(m_projection, m_view);
	render_post_processing(m_projection, m_view, m_model);
	Helpers::Gui::imgui_render();
}

unsigned int Terrain::load_terrain_texture(std::string path)
{
	unsigned int texture;
	int width;
	int height;
	int nrChannels;

	unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

	if (image != nullptr)
	{
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cerr << "Failed to load terrain texture." << std::endl;

	stbi_image_free(image);
	return texture;
}

void Terrain::create_buffers()
{
	//shadow mapping buffer
	const unsigned int shadowTextureWidth = 1024;
	const unsigned int shadowTextureHeight = 1024;

	glGenFramebuffers(1, &shadow_depth_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_depth_FBO);
	glGenTextures(1, &shadow_depth_buffer);
	glBindTexture(GL_TEXTURE_2D, shadow_depth_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth_buffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "shadowDepthFBO is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//reflection buffer
	glGenFramebuffers(1, &reflection_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflection_FBO);
	glGenTextures(1, &reflection_color_buffer);
	glBindTexture(GL_TEXTURE_2D, reflection_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflection_color_buffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "reflectionFBO is incomplete!" << std::endl;

	unsigned int attachmentsReflection[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachmentsReflection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//refraction buffer
	glGenFramebuffers(1, &refraction_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refraction_FBO);
	glGenTextures(1, &refraction_color_buffer);
	glBindTexture(GL_TEXTURE_2D, refraction_color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refraction_color_buffer, 0);

	glGenTextures(1, &refraction_depth_buffer);
	glBindTexture(GL_TEXTURE_2D, refraction_depth_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refraction_depth_buffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "refractionFBO is incomplete!" << std::endl;

	unsigned int attachmentsRefracion[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachmentsRefracion);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//terrain buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &color_buffer);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);

	glGenTextures(1, &normal_buffer);
	glBindTexture(GL_TEXTURE_2D, normal_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_buffer, 0);

	glGenTextures(1, &position_buffer);
	glBindTexture(GL_TEXTURE_2D, position_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, position_buffer, 0);

	glGenTextures(1, &color_buffer_water);
	glBindTexture(GL_TEXTURE_2D, color_buffer_water);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, color_buffer_water, 0);

	glGenTextures(1, &occlusion_buffer);
	glBindTexture(GL_TEXTURE_2D, occlusion_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, occlusion_buffer, 0);

	glGenTextures(1, &depth_buffer);
	glBindTexture(GL_TEXTURE_2D, depth_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer, 0);

	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader_terrain.use_shader();
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), &vertex_data.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(0);
}

void Terrain::render_terrain(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model, View& view, Shader& shader,
	unsigned int fbo, bool reflection_refraction, bool use_empty)
{
	shader.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
	glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "lightProjection"), 1, GL_FALSE, glm::value_ptr(light_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "lightView"), 1, GL_FALSE, glm::value_ptr(light_view));
	glUniform3fv(glGetUniformLocation(shader.get_shader_program(), "tileSize"), 1, glm::value_ptr(tile_size));
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "gridW"), grid_w);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "gridH"), grid_h);

	if (use_empty)
	{
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "outerTessFactor"), refl_refr_tessellation);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "innerTessFactor"), refl_refr_tessellation);
	}
	else
	{
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "outerTessFactor"), outer_tessellation);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "innerTessFactor"), inner_tessellation);
	}

	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "noiseFreq"), noise_frequency);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "noiseOctaves"), noise_octaves);
	glUniform2fv(glGetUniformLocation(shader.get_shader_program(), "translate"), 1, glm::value_ptr(translate));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "heightScale"), height_scale);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "noiseSize"), noise_size);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "cameraLOD"), camera_lod);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "minDepth"), min_depth);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "maxDepth"), max_depth);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "minTessellation"), min_tessellation);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "maxTessellation"), max_tessellation);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "snowBreakpoint"), snow_breakpoint);
	glUniform3fv(glGetUniformLocation(shader.get_shader_program(), "sunPosition"), 1, glm::value_ptr(light_pos));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "shadowBias"), shadow_bias);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "shadowAlpha"), shadow_alpha);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "shadowSamples"), static_cast<float>(shadow_samples));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "shininess"), specular_shininess);
	glUniform3fv(glGetUniformLocation(shader.get_shader_program(), "ambientColor"), 1, glm::value_ptr(ambient_color));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "fogExp"), fog_exp);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "de"), de);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "di"), di);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "fogUpperBound"), fog_upper_bound);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "fogLowerBound"), fog_lower_bound);
	glUniform3fv(glGetUniformLocation(shader.get_shader_program(), "fogColor"), 1, glm::value_ptr(fog_color));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "toggleGrid"), toggle_grid);
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "lineWidth"), line_width);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "cull"), cull);
	glUniform3fv(glGetUniformLocation(shader.get_shader_program(), "lightColor"), 1, glm::value_ptr(light_color));
	glUniform1f(glGetUniformLocation(shader.get_shader_program(), "diffuseAmplitude"), wrapped_diffuse);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "mode"), mode);

	if (reflection_refraction)
	{
		//render reflection texture
		glEnable(GL_CLIP_DISTANCE0);
		mode = 1;
		float distance = (view.get_view_pos().y - water_height) * 2.0f;
		view.set_view_pos(view.get_view_pos() - distance);
		view.invert_pitch();
		glUniformMatrix4fv(glGetUniformLocation(shader.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "mode"), mode);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "outerTessFactor"), refl_refr_tessellation);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "innerTessFactor"), refl_refr_tessellation);
		glBindFramebuffer(GL_FRAMEBUFFER, reflection_FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, noise_texture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grass_texture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snow_texture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sand_texture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliff_texture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadow_depth_buffer);
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		view.set_view_pos(view.get_view_pos() + distance);
		view.invert_pitch();

		//render refraction texture
		mode = 2;
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "mode"), mode);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "outerTessFactor"), outer_tessellation);
		glUniform1i(glGetUniformLocation(shader.get_shader_program(), "innerTessFactor"), inner_tessellation);
		glBindFramebuffer(GL_FRAMEBUFFER, refraction_FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, noise_texture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grass_texture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snow_texture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sand_texture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliff_texture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadow_depth_buffer);
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CLIP_DISTANCE0);
	}

	//render terrain
	mode = 0;
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "mode"), mode);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "outerTessFactor"), outer_tessellation);
	glUniform1i(glGetUniformLocation(shader.get_shader_program(), "innerTessFactor"), inner_tessellation);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, noise_texture);

	if (!use_empty)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grass_texture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snow_texture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sand_texture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliff_texture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadow_depth_buffer);
	}

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::render_post_processing(const glm::mat4& m_projection, const glm::mat4& m_view, const glm::mat4& m_model)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_lighting.use_shader();
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m_model));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.get_shader_program(), "lightProjection"), 1, GL_FALSE, glm::value_ptr(light_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.get_shader_program(), "lightView"), 1, GL_FALSE, glm::value_ptr(light_view));
	glUniform3fv(glGetUniformLocation(shader_lighting.get_shader_program(), "sunPosition"), 1, glm::value_ptr(sun_position));
	glUniform3fv(glGetUniformLocation(shader_lighting.get_shader_program(), "lightPosition"), 1, glm::value_ptr(light_pos));
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "diffuseAmplitude"), wrapped_diffuse);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "de"), de);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "di"), di);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "fogUpperBound"), fog_upper_bound);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "fogLowerBound"), fog_lower_bound);
	glUniform3fv(glGetUniformLocation(shader_lighting.get_shader_program(), "fogColor"), 1, glm::value_ptr(fog_color));
	glUniform3fv(glGetUniformLocation(shader_lighting.get_shader_program(), "ambientColor"), 1, glm::value_ptr(ambient_color));
	glUniform3fv(glGetUniformLocation(shader_lighting.get_shader_program(), "skyColor"), 1, glm::value_ptr(sky_color));
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "shininess"), specular_shininess);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "showSSAO"), show_ssao);
	glUniform1i(glGetUniformLocation(shader_lighting.get_shader_program(), "samples"), samples);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "decay"), decay);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "density"), density);
	glUniform1f(glGetUniformLocation(shader_lighting.get_shader_program(), "weight"), weight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_buffer);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_blur);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, depth_buffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, position_buffer);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, occlusion_buffer);
	Helpers::Draw::draw_quad();
}

void Terrain::render_sun(const glm::mat4& m_projection, const glm::mat4& m_view)
{
	shader_normal.use_shader();
	std::string path = "./Images/sphere/sphere.obj";
	Model model(path);

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::translate(m, light_pos);
	m = glm::scale(m, glm::vec3(0.5f));

	glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "projection"), 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "view"), 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.get_shader_program(), "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3fv(glGetUniformLocation(shader_normal.get_shader_program(), "lightColor"), 1, glm::value_ptr(volume_light_color));
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	model.draw(shader_normal);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}