#ifndef VIEW_H
#define VIEW_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <algorithm>

#include "Helpers.h"

class View
{
public:
	bool		enter_screen;
	bool		fill;
	bool		lock;
	bool		cursor;
	bool		previously_pressed;
	bool		previously_pressed_locked;
	bool		previously_pressed_cursor;
	float		previous_x;
	float		previous_y;
	float		yaw;
	float		pitch;
	float		fov;
	float		velocity;
	float		scroll_sensitivity;
	float		distance;
	glm::vec3	view_pos;
	glm::vec3	view_front;
	glm::vec3	view_up;
	glm::vec3	direction;

public:
	View(int window_width, int window_height, float velocity = 10.0f);
	void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
	void cursor_callback(GLFWwindow* window, double x_pos, double y_pos);
	void process_input(GLFWwindow* window, float deltaTime);
	void lock_view(GLFWwindow* window);
	void show_cursor(GLFWwindow* window);
	void invert_pitch();

	float get_fov() const;
	const glm::vec3& get_view_pos() const;
	const glm::vec3& get_view_front() const;
	const glm::vec3& get_view_up() const;
	void set_view_pos(glm::vec3 pos);
};

#endif