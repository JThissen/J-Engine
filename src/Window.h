#ifndef WINDOW_H
#define WINDOW_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <cctype>
#include <iomanip>
#include <array>

#include "Helpers.h"

class Window
{
protected:
	GLFWwindow*			window;
	int					window_width;
	int					window_height;
	std::string			window_name;
	std::string			glsl_version;

	Window(const std::string& window_name, const std::string& glsl_version);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

public:
	Window() = delete;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
	virtual ~Window();

	void determine_window_size();
	void clear_screen();

	GLFWwindow* get_window() const;
	int get_window_width() const;
	int get_window_height() const;
	const std::string& get_window_name() const;
	const std::string& get_glsl_version() const;
};

#endif