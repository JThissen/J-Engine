#include "Engine.h"

Engine::Engine(const std::string& window_name, const std::string& glsl_version)
	: Window(window_name, glsl_version), current_time(0.0f), previous_time(0.0f), delta_time(0.0f)
{}

Engine& Engine::get_instance(const std::string& window_name, const std::string& glsl_version)
{
	static Engine instance(window_name, glsl_version);
	return instance;
}

void Engine::initialize()
{
	Helpers::Glad::load_glad();
	View view(window_width, window_height);
	initialize_callbacks(view);
	Scene scene(window_width, window_height);
	Helpers::Gui::imgui_initialize(window, glsl_version);

	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::mat4 m_model;

	while (!glfwWindowShouldClose(window))
	{
		m_projection = glm::perspective(view.get_fov(), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
		m_view = glm::lookAt(view.get_view_pos(), view.get_view_pos() + view.get_view_front(), view.get_view_up());
		m_model = glm::mat4(1.0f);

		calc_delta_time();
		glfwPollEvents();
		clear_screen();
		view.process_input(window, delta_time);
		scene.draw_scene(delta_time, m_projection, m_view, m_model, view, Scene::Scenes::TERRAIN);
		glfwSwapBuffers(window);
	}

	Helpers::Gui::imgui_destructor();
}

void Engine::initialize_callbacks(View& camera)
{
	glfwSetWindowUserPointer(window, &camera);
	glfwSetScrollCallback(window, [](GLFWwindow* window, double x_offset, double y_offset)
		{
			View* camera = static_cast<View*>(glfwGetWindowUserPointer(window));
			camera->scroll_callback(window, x_offset, y_offset);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x_pos, double y_pos)
		{
			View* camera = static_cast<View*>(glfwGetWindowUserPointer(window));
			camera->cursor_callback(window, x_pos, y_pos);
		});
}

void Engine::calc_delta_time()
{
	current_time = static_cast<float>(glfwGetTime());
	delta_time = current_time - previous_time;
	previous_time = current_time;
}