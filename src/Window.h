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

class Window
{
protected:
	GLFWwindow*			window;
	int					windowWidth;
	int					windowHeight;
	const std::string	windowName;
	std::string			glsl_version;

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

public:
	Window(const std::string windowName, const std::string glsl_version);
	virtual ~Window();
	void DetermineWindowSize();
	void ClearScreen();
	GLFWwindow*& GetWindow();
	const std::string& GetGLSLversion();
};


#endif