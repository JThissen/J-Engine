#include "Helpers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Helpers
{
	namespace Glad
	{
		void load_glad()
		{
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				throw std::runtime_error("Failed to initialize GLAD");
		}
	}

	namespace Gui
	{
		void imgui_initialize(GLFWwindow*& window, std::string glsl_version)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init(glsl_version.c_str());
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		}

		void imgui_draw_window(int& samples, float& exposure, float& decay, float& density, float& weight)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Volumetric lighting");
			ImGui::Text("Shader parameters ");
			ImGui::SliderInt("samples", &samples, 0, 100);
			ImGui::SliderFloat("exposure", &exposure, 0.0f, 2.0f);
			ImGui::SliderFloat("decay", &decay, 0.0f, 2.0f);
			ImGui::SliderFloat("density", &density, 0.0f, 2.0f);
			ImGui::SliderFloat("weight", &weight, 0.0f, 100.0f);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		void imgui_tessellation_window(int window_width, int window_height, int& grid_w, int& grid_h, glm::vec3& tile_size, int& outer, int& inner, float& noise_freq, int& noise_octaves,
			glm::vec2& translate, float& height_scale, int& noise_size, float& specular_shininess, glm::vec3& ambient_color, float& wrapped_diffuse,
			float& fogExp, float& de, float& di, float& fog_upperbound, float& fog_lowerbound, glm::vec3& fog_color, bool& toggle_grid,
			float& line_width, bool& cull, bool& camera_lod, float& min_depth, float& max_depth, float& min_tessellation, float& max_tessellation,
			glm::vec3& light_color, glm::vec3& sun_position, float& snow_breakpoint, glm::vec3& sky_color, float& shadow_bias, float& shadow_alpha,
			int& shadow_samples, int& samples, float& exposure, float& decay, float& density, float& weight, glm::vec3& volumelight_color)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Terrain & Lighting settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetWindowSize(ImVec2(std::min(static_cast<float>(window_width) * 0.25f, 350.0f), static_cast<float>(window_height) * 0.4f));

			if (ImGui::CollapsingHeader("Terrain"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderInt("Grid width", &grid_w, 0, 256);
				ImGui::SliderInt("Grid height", &grid_h, 0, 256);
				ImGui::SliderFloat3("Tile size", glm::value_ptr(tile_size), 0.0f, 50.0f);
				ImGui::SliderInt("Outer tessellation", &outer, 0, 64);
				ImGui::SliderInt("Inner tessellation", &inner, 0, 64);
				ImGui::SliderFloat("Noise frequency", &noise_freq, 0.0f, 2.0f);
				ImGui::SliderInt("Noise octaves", &noise_octaves, 0, 36);
				ImGui::SliderFloat2("Translate", glm::value_ptr(translate), 0.0f, 50.0f);
				ImGui::SliderFloat("Height scale", &height_scale, 0.0f, 25.0f);
				ImGui::SliderFloat("Snow", &snow_breakpoint, 0.0f, 5.0f);
				ImGui::SliderInt("Noise size", &noise_size, 0, 512);
				ImGui::Checkbox("Show grid", &toggle_grid);
				ImGui::SliderFloat("Line width (grid)", &line_width, 0.0f, 0.05f);
				ImGui::Checkbox("Camera LOD", &camera_lod);

				if (camera_lod)
				{
					ImGui::SliderFloat("Min. tessellation", &min_tessellation, 0.0f, 64.0f);
					ImGui::SliderFloat("Max. tessellation", &max_tessellation, 0.0f, 64.0f);
					ImGui::SliderFloat("Min. camera depth", &min_depth, 0.0f, 100.0f);
					ImGui::SliderFloat("Max. camera depth", &max_depth, 0.0f, 100.0f);
				}
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Lighting"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat("Specular shininess", &specular_shininess, 0.0f, 1.0f);
				ImGui::SliderFloat3("Ambient color", glm::value_ptr(ambient_color), 0.0f, 1.0f);
				ImGui::SliderFloat3("Light color", glm::value_ptr(light_color), 0.0f, 2.0f);
				ImGui::SliderFloat("Diffuse", &wrapped_diffuse, 0.0f, 2.0f);
				ImGui::SliderFloat3("Sun position", glm::value_ptr(sky_color), -100.0f, 100.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Shadow"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat("Shadow bias", &shadow_bias, 0.0f, 0.05f);
				ImGui::SliderFloat("Shadow alpha", &shadow_alpha, 0.0f, 2.0f);
				ImGui::SliderInt("Shadow samples", &shadow_samples, 0, 25);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Fog"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat3("Fog color", glm::value_ptr(fog_color), 0.0f, 1.0f);
				ImGui::SliderFloat("Extinction", &de, 0.0f, 1.0f);
				ImGui::SliderFloat("Inscattering", &di, 0.0f, 1.0f);
				ImGui::SliderFloat("Fog upper bound", &fog_upperbound, 0.0f, 25.0f);
				ImGui::SliderFloat("Fog lower bound", &fog_lowerbound, 0.0f, 25.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Volumetric light"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderInt("Samples", &samples, 0, 500);
				ImGui::SliderFloat("Exposure", &exposure, 0.0f, 0.1f);
				ImGui::SliderFloat("Decay", &decay, 0.0f, 1.0f);
				ImGui::SliderFloat("Density", &density, 0.0f, 1.0f);
				ImGui::SliderFloat("Weight", &weight, 0.0f, 1.0f);
				ImGui::SliderFloat3("Color", glm::value_ptr(volumelight_color), 0.0f, 1.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text("Press E to unlock/lock the camera.");
			ImGui::Text("Press R to show/hide the cursor.");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS).", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		void imgui_water(int window_width, int window_height, float& height_min, float& height_max, glm::vec2& dir1, glm::vec2& dir2, glm::vec2& dir3, glm::vec2& dir4, float& wavelength1,
			float& wavelength2, float& wavelength3, float& wavelength4, float& amplitude1, float& amplitude2, float& amplitude3, float& amplitude4,
			float& speed1, float& speed2, float& speed3, float& speed4, int& inner_tessellation_water, int& outer_tessellation_water,
			float& specular_strength, float& mix_strength, glm::vec3& shallow_color, glm::vec3& deep_color, bool& toggle_grid, float& line_width)
		{
			ImGui::Begin("Water settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, static_cast<float>(window_height) * 0.4f));
			ImGui::SetWindowSize(ImVec2(std::min(static_cast<float>(window_width) * 0.25f, 350.0f), static_cast<float>(window_height * 0.4f)));

			if (ImGui::CollapsingHeader("Water"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::Checkbox("Show grid", &toggle_grid);
				ImGui::SliderFloat("Line width (grid)", &line_width, 0.0f, 0.05f);
				ImGui::SliderInt("Outer tessellation", &outer_tessellation_water, 0, 64);
				ImGui::SliderInt("Inner tessellation", &inner_tessellation_water, 0, 64);
				ImGui::SliderFloat("Height min.", &height_min, -10.0f, 0.0f);
				ImGui::SliderFloat("Height max.", &height_max, 0.0f, 10.0f);
				ImGui::SliderFloat2("dir1", glm::value_ptr(dir1), -1.0f, 1.0f);
				ImGui::SliderFloat2("dir2", glm::value_ptr(dir2), -1.0f, 1.0f);
				ImGui::SliderFloat2("dir3", glm::value_ptr(dir3), -1.0f, 1.0f);
				ImGui::SliderFloat2("dir4", glm::value_ptr(dir4), -1.0f, 1.0f);
				ImGui::SliderFloat("wavelength1", &wavelength1, 0.0f, 10.0f);
				ImGui::SliderFloat("wavelength2", &wavelength2, 0.0f, 10.0f);
				ImGui::SliderFloat("wavelength3", &wavelength3, 0.0f, 10.0f);
				ImGui::SliderFloat("wavelength4", &wavelength4, 0.0f, 10.0f);
				ImGui::SliderFloat("amplitude1", &amplitude1, 0.0f, 0.1f);
				ImGui::SliderFloat("amplitude2", &amplitude2, 0.0f, 0.1f);
				ImGui::SliderFloat("amplitude3", &amplitude3, 0.0f, 0.1f);
				ImGui::SliderFloat("amplitude4", &amplitude4, 0.0f, 0.1f);
				ImGui::SliderFloat("speed1", &speed1, -10.0f, 10.0f);
				ImGui::SliderFloat("speed2", &speed2, -10.0f, 10.0f);
				ImGui::SliderFloat("speed3", &speed3, -10.0f, 10.0f);
				ImGui::SliderFloat("speed4", &speed4, -10.0f, 10.0f);
				ImGui::SliderFloat("specular strength", &specular_strength, 0.0f, 1.0f);
				ImGui::SliderFloat("mix strength", &mix_strength, 0.0f, 0.1f);
				ImGui::SliderFloat3("shallow color", glm::value_ptr(shallow_color), 0.0f, 1.0f);
				ImGui::SliderFloat3("deep color", glm::value_ptr(deep_color), 0.0f, 1.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}
			ImGui::End();
		}

		void imgui_ssao(int window_width, int window_height, int& kernel_size, float& radius, float& bias, bool& blur_SSAO, bool& show_SSAO)
		{
			ImGui::Begin("SSAO settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, static_cast<float>(window_height) * 0.8f));
			ImGui::SetWindowSize(ImVec2(std::min(static_cast<float>(window_width) * 0.25f, 350.0f), static_cast<float>(window_height * 0.2f)));

			if (ImGui::CollapsingHeader("SSAO"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderInt("Kernel size", &kernel_size, 0, 256);
				ImGui::SliderFloat("Radius", &radius, 0.0f, 5.0f);
				ImGui::SliderFloat("Bias", &bias, 0.0f, 0.1f);
				ImGui::Checkbox("Blur SSAO", &blur_SSAO);
				ImGui::Checkbox("Show SSAO", &show_SSAO);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}
			ImGui::End();
		}

		void imgui_destructor()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void imgui_render()
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	namespace Draw
	{
		void draw_point()
		{
			if (VBO_point == 0)
			{
				Vertex v{ 0.0f, 0.0f, 0.0f };

				glGenVertexArrays(1, &VAO_point);
				glGenBuffers(1, &VBO_point);
				glBindVertexArray(VAO_point);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_point);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), &v, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO_point);
			glDrawArrays(GL_POINTS, 0, 3);
			glBindVertexArray(0);
		}

		void draw_horizontal_line(float width, float start, float end)
		{
			std::array<Vertex, 2> vertices;

			if (VBO_hor_line == 0)
			{
				vertices = { Vertex{start, 0.0f, 0.0f},
							 Vertex{end, 0.0f, 0.0f} };

				glGenVertexArrays(1, &VAO_hor_line);
				glGenBuffers(1, &VBO_hor_line);
				glBindVertexArray(VAO_hor_line);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_hor_line);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO_hor_line);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		void draw_vertical_line(float start, float end)
		{
			std::array<Vertex, 2> vertices;

			if (VBO_ver_line == 0)
			{
				vertices = { Vertex{0.0f, start, 0.0f},
							 Vertex{0.0f, end, 0.0f} };

				glGenVertexArrays(1, &VAO_ver_line);
				glGenBuffers(1, &VBO_ver_line);
				glBindVertexArray(VAO_ver_line);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_ver_line);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO_ver_line);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		void draw_cube()
		{
			if (VBO_cube == 0)
			{
				float vertices[] = {
					-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
					 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
					 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
					 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
					-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
					-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

					-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
					 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
					 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
					-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
					-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

					-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
					-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
					-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
					-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
					-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
					-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

					 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
					 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
					 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
					 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
					 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
					 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

					-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
					 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
					 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
					 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
					-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
					-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

					-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
					 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
					 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
					 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
					-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
					-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
				};

				glGenVertexArrays(1, &VAO_cube);
				glGenBuffers(1, &VBO_cube);
				glBindVertexArray(VAO_cube);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(sizeof(float) * 3));
				glEnableVertexAttribArray(1);
			}

			glBindVertexArray(VAO_cube);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		void draw_line(const Vertex& v1, const Vertex& v2)
		{
			std::array<Vertex, 2> vertices;

			if (VBO_line == 0)
			{
				vertices = { v1, v2 };
				glGenVertexArrays(1, &VAO_line);
				glGenBuffers(1, &VBO_line);
				glBindVertexArray(VAO_line);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO_line);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		void draw_quad()
		{
			if (VBO_quad == 0)
			{
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};

				glGenVertexArrays(1, &VAO_quad);
				glGenBuffers(1, &VBO_quad);
				glBindVertexArray(VAO_quad);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
			}
			glBindVertexArray(VAO_quad);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

		void draw_plane()
		{
			if (VBO_plane == 0)
			{
				float planeVertices[] = {
					 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
					-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
					-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

					 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
					-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
					 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
				};

				glGenVertexArrays(1, &VAO_plane);
				glGenBuffers(1, &VBO_plane);
				glBindVertexArray(VAO_plane);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_plane);
				glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 3));
			}
			glBindVertexArray(VAO_plane);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		void draw_skybox_cube()
		{
			if (VBO_skybox_cube == 0)
			{
				float vertices[] = {
					-1.0f,  1.0f, -1.0f,
					-1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,

					-1.0f, -1.0f,  1.0f,
					-1.0f, -1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f, -1.0f,
					-1.0f,  1.0f,  1.0f,
					-1.0f, -1.0f,  1.0f,

					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,

					-1.0f, -1.0f,  1.0f,
					-1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f, -1.0f,  1.0f,
					-1.0f, -1.0f,  1.0f,

					-1.0f,  1.0f, -1.0f,
					 1.0f,  1.0f, -1.0f,
					 1.0f,  1.0f,  1.0f,
					 1.0f,  1.0f,  1.0f,
					-1.0f,  1.0f,  1.0f,
					-1.0f,  1.0f, -1.0f,

					-1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f,  1.0f,
					 1.0f, -1.0f, -1.0f,
					 1.0f, -1.0f, -1.0f,
					-1.0f, -1.0f,  1.0f,
					 1.0f, -1.0f,  1.0f
				};

				glGenVertexArrays(1, &VAO_skybox_cube);
				glGenBuffers(1, &VBO_skybox_cube);
				glBindVertexArray(VAO_skybox_cube);
				glBindBuffer(GL_ARRAY_BUFFER, VBO_skybox_cube);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO_skybox_cube);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	namespace Load
	{
		unsigned int load_texture(std::string path)
		{
			unsigned int texture;
			int width;
			int height;
			int nrChannels;

			unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

			if (image != nullptr)
			{
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
				std::cerr << "Failed to load texture." << std::endl;

			stbi_image_free(image);
			return texture;
		}

		unsigned int load_cubemap(std::vector<std::string> faces)
		{
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

			int width;
			int height;
			int nrChannels;

			for (size_t i = 0; i < faces.size(); i++)
			{
				unsigned char* image = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

				if (image != nullptr)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				else
					std::cerr << "Failed to load cube map texture: " << i << "." << std::endl;

				stbi_image_free(image);
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			return texture;
		}
	}

	namespace Print
	{
		std::ostream& operator<< (std::ostream& os, glm::vec3 vec)
		{
			os << vec.x << ", " << vec.y << ", " << vec.z;
			return os;
		}
	}

	namespace Noise
	{
		
	}

	namespace Utility
	{
		void to_lower_case(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](char c)
				{
					return std::tolower(c);
				});
		}

		bool all_digits(const std::string& str)
		{
			return str.find_first_not_of("0123456789") == -1;
		}
	}
}