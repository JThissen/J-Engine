#include "View.h"

View::View(int window_width, int window_height, float velocity)
{
	enter_screen = true;
	fill = true;
	lock = false;
	cursor = false;
	previous_x = window_width / 2.0f;
	previous_y = window_height / 2.0f;
	yaw = -63.8f;
	pitch = -23.5f;
	fov = 45.0f;
	scroll_sensitivity = 0.1f;
	this->velocity = velocity;
	view_pos = glm::vec3(-8.4f, 7.6f, 14.2f);
	view_front = glm::vec3(0.0f, 0.0f, -1.0f);
	view_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void View::scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
	fov -= static_cast<float>(y_offset);
	Helpers::Utility::clamp<float>(fov, 1.0f, 45.0f);
}

void View::cursor_callback(GLFWwindow* window, double x_pos, double y_pos)
{
	if (cursor)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!lock)
	{
		float x_pos_f = static_cast<float>(x_pos);
		float y_pos_f = static_cast<float>(y_pos);

		if (enter_screen)
		{
			previous_x = x_pos_f;
			previous_y = y_pos_f;
			enter_screen = false;
		}

		float x_offset = x_pos_f - previous_x;
		float y_offset = previous_y - y_pos_f;

		previous_x = x_pos_f;
		previous_y = y_pos_f;

		x_offset *= scroll_sensitivity;
		y_offset *= scroll_sensitivity;

		yaw += x_offset;
		pitch += y_offset;

		Helpers::Utility::clamp<float>(pitch, -89.0f, 89.0f);
		
		float radius = 1.0f;
		direction.x = radius * cos(glm::radians(pitch)) * radius * cos(glm::radians(yaw));
		direction.y = radius * sin(glm::radians(pitch));
		direction.z = radius * cos(glm::radians(pitch)) * radius * sin(glm::radians(yaw));
		view_front = glm::normalize(direction);
	}
}

void View::process_input(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	lock_view(window);
	show_cursor(window);

	distance = deltaTime * velocity;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		view_pos += distance * view_front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		view_pos += distance * -view_front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		view_pos += distance * -glm::normalize(glm::cross(view_front, view_up));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		view_pos += distance * glm::normalize(glm::cross(view_front, view_up));
}

void View::lock_view(GLFWwindow* window)
{
	bool currently_pressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

	if (currently_pressed && !previously_pressed_locked)
		lock = !lock;

	previously_pressed_locked = currently_pressed;
}

void View::show_cursor(GLFWwindow* window)
{
	bool currently_pressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

	if (currently_pressed && !previously_pressed_cursor)
		cursor = !cursor;

	previously_pressed_cursor = currently_pressed;
}

float View::get_fov() const { return fov; }
const glm::vec3& View::get_view_pos() const { return view_pos; }
const glm::vec3& View::get_view_front() const { return view_front; }
const glm::vec3& View::get_view_up() const { return view_up; }
void View::set_view_pos(glm::vec3 pos) { view_pos = pos; }
void View::invert_pitch() { pitch = -pitch; }