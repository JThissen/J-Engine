#include "Helpers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Helpers
{
	namespace Glad
	{
		void LoadGlad()
		{
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				std::runtime_error("Failed to initialize GLAD");
		}
	}

	namespace Gui
	{
		void ImGuiInitialize(GLFWwindow*& window, std::string glsl_version)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init(glsl_version.c_str());
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		}

		void ImGuiDrawWindow(int& samples, float& exposure, float& decay, float& density, float& weight)
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

		void ImGuiTessellationWindow(int windowWidth, int windowHeight, int& gridW, int& gridH, glm::vec3& tileSize, int& outer, int& inner, float& noiseFreq, int& noiseOctaves,
			glm::vec2& translate, float& heightScale, int& noiseSize, float& specularShininess, glm::vec3& ambientColor, float& wrappedDiffuse,
			float& fogExp, float& de, float& di, float& fogUpperBound, float& fogLowerBound, glm::vec3& fogColor, bool& toggleGrid,
			float& lineWidth, bool& cull, bool& cameraLOD, float& minDepth, float& maxDepth, float& minTessellation, float& maxTessellation,
			glm::vec3& lightColor, glm::vec3& sunPosition, float& snowBreakpoint, glm::vec3& skyColor, float& shadowBias, float& shadowAlpha, 
			int& shadowSamples, int& samples, float& exposure, float& decay, float& density, float& weight, glm::vec3& volumeLightColor)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Terrain & Lighting settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetWindowSize(ImVec2(static_cast<float>(windowWidth) * 0.25f, static_cast<float>(windowHeight) * 0.4f));

			if (ImGui::CollapsingHeader("Terrain"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderInt("Grid width", &gridW, 0, 256);
				ImGui::SliderInt("Grid height", &gridH, 0, 256);
				ImGui::SliderFloat3("Tile size", glm::value_ptr(tileSize), 0.0f, 50.0f);
				ImGui::SliderInt("Outer tessellation", &outer, 0, 64);
				ImGui::SliderInt("Inner tessellation", &inner, 0, 64);
				ImGui::SliderFloat("Noise frequency", &noiseFreq, 0.0f, 2.0f);
				ImGui::SliderInt("Noise octaves", &noiseOctaves, 0, 36);
				ImGui::SliderFloat2("Translate", glm::value_ptr(translate), 0.0f, 50.0f);
				ImGui::SliderFloat("Height scale", &heightScale, 0.0f, 25.0f);
				ImGui::SliderFloat("Snow", &snowBreakpoint, 0.0f, 5.0f);
				ImGui::SliderInt("Noise size", &noiseSize, 0, 512);
				ImGui::Checkbox("Show grid", &toggleGrid);
				ImGui::SliderFloat("Line width (grid)", &lineWidth, 0.0f, 0.05f);
				ImGui::Checkbox("Camera LOD", &cameraLOD);

				if (cameraLOD)
				{
					ImGui::SliderFloat("Min. tessellation", &minTessellation, 0.0f, 64.0f);
					ImGui::SliderFloat("Max. tessellation", &maxTessellation, 0.0f, 64.0f);
					ImGui::SliderFloat("Min. camera depth", &minDepth, 0.0f, 100.0f);
					ImGui::SliderFloat("Max. camera depth", &maxDepth, 0.0f, 100.0f);
				}
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Lighting"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat("Specular shininess", &specularShininess, 0.0f, 1.0f);
				ImGui::SliderFloat3("Ambient color", glm::value_ptr(ambientColor), 0.0f, 1.0f);
				ImGui::SliderFloat3("Light color", glm::value_ptr(lightColor), 0.0f, 2.0f);
				ImGui::SliderFloat("Diffuse", &wrappedDiffuse, 0.0f, 2.0f);
				ImGui::SliderFloat3("Sun position", glm::value_ptr(skyColor), -100.0f, 100.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Shadow"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat("Shadow bias", &shadowBias, 0.0f, 0.05f);
				ImGui::SliderFloat("Shadow alpha", &shadowAlpha, 0.0f, 2.0f);
				ImGui::SliderInt("Shadow samples", &shadowSamples, 0, 25);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			if (ImGui::CollapsingHeader("Fog"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderFloat3("Fog color", glm::value_ptr(fogColor), 0.0f, 1.0f);
				ImGui::SliderFloat("Extinction", &de, 0.0f, 1.0f);
				ImGui::SliderFloat("Inscattering", &di, 0.0f, 1.0f);
				ImGui::SliderFloat("Fog upper bound", &fogUpperBound, 0.0f, 25.0f);
				ImGui::SliderFloat("Fog lower bound", &fogLowerBound, 0.0f, 25.0f);
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
				ImGui::SliderFloat3("Color", glm::value_ptr(volumeLightColor), 0.0f, 1.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}

			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Text("Press E to lock the camera.");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS).", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		void ImGuiWater(int windowWidth, int windowHeight, float& heightMin, float& heightMax, glm::vec2& dir1, glm::vec2& dir2, glm::vec2& dir3, glm::vec2& dir4, float& wavelength1,
			float& wavelength2, float& wavelength3, float& wavelength4, float& amplitude1, float& amplitude2, float& amplitude3, float& amplitude4,
			float& speed1, float& speed2, float& speed3, float& speed4, int& innerTessellationWater, int& outerTessellationWater,
			float& specularStrength, float& mixStrength, glm::vec3& shallowColor, glm::vec3& deepColor, bool& toggleGrid, float& lineWidth)
		{
			ImGui::Begin("Water settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, static_cast<float>(windowHeight) * 0.4f));
			ImGui::SetWindowSize(ImVec2(static_cast<float>(windowWidth) * 0.25f, static_cast<float>(windowHeight * 0.4f)));

			if (ImGui::CollapsingHeader("Water"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::Checkbox("Show grid", &toggleGrid);
				ImGui::SliderFloat("Line width (grid)", &lineWidth, 0.0f, 0.05f);
				ImGui::SliderInt("Outer tessellation", &outerTessellationWater, 0, 64);
				ImGui::SliderInt("Inner tessellation", &innerTessellationWater, 0, 64);
				ImGui::SliderFloat("Height min.", &heightMin, -10.0f, 0.0f);
				ImGui::SliderFloat("Height max.", &heightMax, 0.0f, 10.0f);
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
				ImGui::SliderFloat("specular strength", &specularStrength, 0.0f, 1.0f);
				ImGui::SliderFloat("mix strength", &mixStrength, 0.0f, 0.1f);
				ImGui::SliderFloat3("shallow color", glm::value_ptr(shallowColor), 0.0f, 1.0f);
				ImGui::SliderFloat3("deep color", glm::value_ptr(deepColor), 0.0f, 1.0f);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}
			ImGui::End();
		}

		void ImGuiSSAO(int windowWidth, int windowHeight, int& kernelSize, float& radius, float&bias, bool& blurSSAO, bool& showSSAO)
		{
			ImGui::Begin("SSAO settings");
			ImGui::SetWindowCollapsed(false);
			ImGui::SetWindowPos(ImVec2(0.0f, static_cast<float>(windowHeight) * 0.8f));
			ImGui::SetWindowSize(ImVec2(static_cast<float>(windowWidth) * 0.25f, static_cast<float>(windowHeight * 0.2f)));

			if (ImGui::CollapsingHeader("SSAO"))
			{
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
				ImGui::SliderInt("Kernel size", &kernelSize, 0, 256);
				ImGui::SliderFloat("Radius", &radius, 0.0f, 5.0f);
				ImGui::SliderFloat("Bias", &bias, 0.0f, 0.1f);
				ImGui::Checkbox("Blur SSAO", &blurSSAO);
				ImGui::Checkbox("Show SSAO", &showSSAO);
				ImGui::Dummy(ImVec2(0.0f, 10.0f));
			}
			ImGui::End();
		}

		void ImGuiDestructor()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void ImGuiRender()
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	namespace Draw
	{
		void drawPoint(float size)
		{
			glPointSize(size);
			unsigned int VAO = 0;
			unsigned int VBO = 0;

			if (VAO == 0)
			{
				Vertex v{ 0.0f, 0.0f, 0.0f };

				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex), &v, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, 3);
			glBindVertexArray(0);
		}

		void drawHorizontalLine(float width, float start, float end)
		{
			unsigned int VAO = 0;
			unsigned int VBO = 0;
			std::array<Vertex, 2> vertices;

			if (VAO == 0)
			{
				vertices = { Vertex{start, 0.0f, 0.0f},
							 Vertex{end, 0.0f, 0.0f} };

				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		void drawVerticalLine(float width, float start, float end)
		{
			unsigned int VAO = 0;
			unsigned int VBO = 0;
			std::array<Vertex, 2> vertices;

			if (VAO == 0)
			{
				vertices = { Vertex{0.0f, start, 0.0f},
							 Vertex{0.0f, end, 0.0f} };

				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		void drawCube()
		{
			unsigned int VAO = 0;
			unsigned int VBO = 0;

			if (VAO == 0)
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

				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
				glEnableVertexAttribArray(1);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		void drawLine(float width, Vertex v1, Vertex v2)
		{
			glLineWidth(width);
			unsigned int VAO = 0;
			unsigned int VBO = 0;
			std::array<Vertex, 2> vertices;

			if (VAO == 0)
			{
				vertices = { v1, v2 };
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_LINES, 0, vertices.size());
			glBindVertexArray(0);
		}

		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		void drawQuad()
		{
			if (quadVAO == 0)
			{
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};

				glGenVertexArrays(1, &quadVAO);
				glGenBuffers(1, &quadVBO);
				glBindVertexArray(quadVAO);
				glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

		unsigned int planeVAO = 0;
		unsigned int planeVBO;

		void drawPlane()
		{
			if (planeVAO == 0)
			{
				float planeVertices[] = {
					 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
					-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
					-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

					 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
					-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
					 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
				};

				glGenVertexArrays(1, &planeVAO);
				glGenBuffers(1, &planeVBO);
				glBindVertexArray(planeVAO);
				glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
			}
			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		void drawSkyboxCube()
		{
			unsigned int VAO = 0;
			unsigned int VBO = 0;

			if (VAO == 0)
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

				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
				glEnableVertexAttribArray(0);
			}

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}

	namespace Load
	{
		unsigned int loadTexture(std::string path)
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

		unsigned int loadCubemap(std::vector<std::string> faces)
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

	namespace Globals
	{
		
	}

	namespace Noise
	{
		unsigned int createNoiseTexture2D(GLint internalFormat, bool mipmap)
		{
			constexpr int size = 300;
			std::array<float, size * size> data;
			for (int y = 0; y < size; y++)
			{
				for (int x = 0; x < size; x++)
				{
					data[y*size + x] = rand() / (float)RAND_MAX;
				}
			}

			unsigned int textureNoise;
			glGenTextures(1, &textureNoise);
			glBindTexture(GL_TEXTURE_2D, textureNoise);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size, size, 0, GL_RED, GL_FLOAT, data.data());
			if (mipmap) {
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			return textureNoise;
		}
	}
}