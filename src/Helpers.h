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

namespace Helpers
{
	namespace Glad
	{
		void LoadGlad();
	}
	
	namespace Gui
	{
		void ImGuiInitialize(GLFWwindow*& window, std::string glsl_version);
		void ImGuiDrawWindow(int& samples, float& exposure, float& decay, float& density, float& weight);
		void ImGuiTessellationWindow(int windowWidth, int windowHeight, int& gridW, int& gridH, glm::vec3& tileSize, int& outer, int& inner, float& noiseFreq, int& noiseOctaves,
			glm::vec2& translate, float& heightScale, int& noiseSize, float& specularShininess, glm::vec3& ambientColor, float& wrappedDiffuse,
			float& fogExp, float& de, float& di, float& fogUpperBound, float& fogLowerBound, glm::vec3& fogColor, bool& toggleGrid,
			float& lineWidth, bool& cull, bool& cameraLOD, float& minDepth, float& maxDepth, float& minTessellation, float& maxTessellation,
			glm::vec3& lightColor, glm::vec3& sunPosition, float& snowBreakpoint, glm::vec3& skyColor, float& shadowBias, float& shadowAlpha, 
			int& shadowSamples, int& samples, float& exposure, float& decay, float& density, float& weight, glm::vec3& volumeLightColor);

		void ImGuiWater(int windowWidth, int windowHeight, float& heightMin, float& heightMax, glm::vec2& dir1, glm::vec2& dir2, glm::vec2& dir3, glm::vec2& dir4, float& wavelength1,
			float& wavelength2, float& wavelength3, float& wavelength4, float& amplitude1, float& amplitude2, float& amplitude3, float& amplitude4,
			float& speed1, float& speed2, float& speed3, float& speed4, int& innerTessellationWater, int& outerTessellationWater,
			float& specularStrength, float& mixStrength, glm::vec3& shallowColor, glm::vec3& deepColor, bool& toggleGrid, float& lineWidth);

		void ImGuiSSAO(int windowWidth, int windowHeight, int& kernelSize, float& radius, float&bias, bool& blurSSAO, bool& showSSAO);

		void ImGuiDestructor();
		void ImGuiRender();
	}

	namespace Draw
	{
		typedef struct
		{
			float x, y, z;
		} Vertex;

		void drawPoint(float size);
		void drawHorizontalLine(float width, float start, float end);
		void drawVerticalLine(float width, float start, float end);
		void drawCube();
		void drawLine(float width, Vertex v1, Vertex v2);
		void drawQuad();
		void drawPlane();
		void drawSkyboxCube();
	}

	namespace Load
	{
		unsigned int loadTexture(std::string path);
		unsigned int loadCubemap(std::vector<std::string> faces);
	}

	namespace Print
	{
		std::ostream& operator<< (std::ostream& os, glm::vec3 vec);
	}

	namespace Globals
	{
		static bool toggleTerrain = true;
		static bool showCursor = true;
	}

	namespace Noise
	{
		unsigned int createNoiseTexture2D(GLint internalFormat, bool mipmap);
	}
}

#endif