#include "Engine.h"
#include "VolumeLight.h"
#include "Camera.h"
#include "Scene.h"
#include "Helpers.h"

Engine& Engine::CreateInstance(const std::string& windowName, const std::string& glslVersion)
{
	static Engine e(windowName, glslVersion);
	return e;
}

Engine::Engine(const std::string& windowName, const std::string& glsl_version)
	: Window(windowName, glsl_version), currentTime(0.0f), previousTime(0.0f), deltaTime(0.0f) {}

void Engine::Initialize()
{
	Helpers::Glad::LoadGlad();
	Camera camera(windowWidth, windowHeight);
	glfwSetWindowUserPointer(window, &camera);
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		camera->scrollCallback(window, xoffset, yoffset);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
	{
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		camera->cursorCallback(window, xpos, ypos);
	});

	Scene scene(windowWidth, windowHeight);
	Helpers::Gui::ImGuiInitialize(window, glsl_version);

	while (!glfwWindowShouldClose(window))
	{
		CalcDeltaTime();
		glfwPollEvents();
		camera.processInput(window, deltaTime);
		ClearScreen();

		glm::mat4 projection = glm::perspective(camera.Getfov(), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camera.GetCameraPos(), camera.GetCameraPos() + camera.GetCameraFront(), camera.GetCameraUp());
		glm::mat4 model = glm::mat4(1.0f);

		scene.DrawScene(deltaTime, projection, view, model, camera, Scene::Scenes::TERRAIN);

		glfwSwapBuffers(window);
	}

	Helpers::Gui::ImGuiDestructor();
}

void Engine::CalcDeltaTime()
{
	currentTime = static_cast<float>(glfwGetTime());
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
}