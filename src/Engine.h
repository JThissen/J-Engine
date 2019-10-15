#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>
#include <random>

#include "Window.h"

class Engine : public Window
{
private:
	float		currentTime;
	float		previousTime;
	float		deltaTime;

	Engine(const std::string& windowName, const std::string& glsl_version);

public:
	Engine() = delete;
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;
	virtual ~Engine() = default;

	static Engine& CreateInstance(const std::string& windowName, const std::string& glslVersion);
	void Initialize();
	void CalcDeltaTime();
};

#endif