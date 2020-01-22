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
#include "VolumeLight.h"
#include "View.h"
#include "Scene.h"
#include "Helpers.h"

class Engine : public Window
{
private:
	float		current_time;
	float		previous_time;
	float		delta_time;

	Engine(const std::string& window_name, const std::string& glsl_version);

public:
	Engine() = delete;
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;
	virtual ~Engine() = default;

	static Engine& get_instance(const std::string& window_name, const std::string& glsl_version);
	void initialize();
	void initialize_callbacks(View& camera);
	void calc_delta_time();
	void look_at(glm::vec3 view_pos, glm::vec3 view_front, glm::vec3 view_up = glm::vec3(0.0f, 1.0f, 0.0f));
};

#endif