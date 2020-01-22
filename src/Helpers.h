#ifndef HELPERS_H
#define HELPERS_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <iostream>
#include <ostream>
#include <array>
#include <vector>
#include <memory>
#include <cctype>

namespace Helpers
{
	namespace Glad
	{
		void load_glad();
	}
	
	namespace Gui
	{
		void imgui_initialize(GLFWwindow*& window, std::string glsl_version);
		void imgui_draw_window(int& samples, float& exposure, float& decay, float& density, float& weight);
		void imgui_tessellation_window(int window_width, int window_height, int& grid_w, int& grid_h, glm::vec3& tile_size, int& outer, int& inner, float& noise_freq, int& noise_octaves,
			glm::vec2& translate, float& height_scale, int& noise_size, float& specular_shininess, glm::vec3& ambient_color, float& wrapped_diffuse,
			float& fog_exp, float& de, float& di, float& fog_upperbound, float& fog_lowerbound, glm::vec3& fog_color, bool& toggle_grid,
			float& line_width, bool& cull, bool& camera_lod, float& min_depth, float& max_depth, float& min_tessellation, float& max_tessellation,
			glm::vec3& light_color, glm::vec3& sun_position, float& snow_breakpoint, glm::vec3& sky_color, float& shadow_bias, float& shadow_alpha, 
			int& shadow_samples, int& samples, float& exposure, float& decay, float& density, float& weight, glm::vec3& volumelight_color);

		void imgui_water(int window_width, int window_height, float& height_min, float& height_max, glm::vec2& dir1, glm::vec2& dir2, glm::vec2& dir3, glm::vec2& dir4, float& wavelength1,
			float& wavelength2, float& wavelength3, float& wavelength4, float& amplitude1, float& amplitude2, float& amplitude3, float& amplitude4,
			float& speed1, float& speed2, float& speed3, float& speed4, int& inner_tessellation_water, int& outer_tessellation_water,
			float& specular_strength, float& mix_strength, glm::vec3& shallow_color, glm::vec3& deep_color, bool& toggle_grid, float& line_width);

		void imgui_ssao(int window_width, int window_height, int& kernel_size, float& radius, float&bias, bool& blur_SSAO, bool& show_SSAO);

		void imgui_destructor();
		void imgui_render();
	}

	namespace Draw
	{
		namespace
		{
			unsigned int VAO_point = 0;
			unsigned int VBO_point = 0;

			unsigned int VAO_line = 0;
			unsigned int VBO_line = 0;

			unsigned int VAO_hor_line = 0;
			unsigned int VBO_hor_line = 0;

			unsigned int VAO_ver_line = 0;
			unsigned int VBO_ver_line = 0;

			unsigned int VAO_cube = 0;
			unsigned int VBO_cube = 0;

			unsigned int VAO_quad = 0;
			unsigned int VBO_quad = 0;

			unsigned int VAO_plane = 0;
			unsigned int VBO_plane = 0;

			unsigned int VAO_skybox_cube = 0;
			unsigned int VBO_skybox_cube = 0;
		}

		typedef struct
		{
			float x, y, z;
		} Vertex;

		void draw_point();
		void draw_horizontal_line(float start, float end);
		void draw_vertical_line(float start, float end);
		void draw_cube();
		void draw_line(const Vertex& v1, const Vertex& v2);
		void draw_quad();
		void draw_plane();
		void draw_skybox_cube();
	}

	namespace Load
	{
		unsigned int load_texture(std::string path);
		unsigned int load_cubemap(std::vector<std::string> faces);
	}

	namespace Print
	{
		std::ostream& operator<< (std::ostream& os, glm::vec3 vec);
	}

	namespace Noise
	{
		template<std::size_t size>
		unsigned int create_noise_texture_2d(GLint internal_format, bool mipmap)
		{
			unsigned int textureNoise;
			float* data = new float[size * size];
			for (int y = 0; y < size; y++)
			{
				for (int x = 0; x < size; x++)
					data[y * size + x] = rand() / static_cast<float>(RAND_MAX);
			}

			glGenTextures(1, &textureNoise);
			glBindTexture(GL_TEXTURE_2D, textureNoise);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size, size, 0, GL_RED, GL_FLOAT, data);

			if (mipmap)
				glGenerateMipmap(GL_TEXTURE_2D);

			return textureNoise;
		}
	}

	namespace Utility
	{
		void to_lower_case(std::string& str);
		bool all_digits(const std::string& str);

		template<typename T>
		const T& clamp(const T& value, const T& min, const T& max)
		{
			return (value < min) ? min : (value > max) ? max : value;
		}

		template<typename T>
		float lerp(T a, T b, T c)
		{
			return (b - a) * c + a;
		}
	}
}

#endif