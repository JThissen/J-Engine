#include "Window.h"
#include "Helpers.h"

Window::Window(const std::string windowName, const std::string glsl_version)
	: windowName(windowName), glsl_version(glsl_version)
{
	if (!glfwInit())
		std::runtime_error("GLFW failed to initialize.");

	DetermineWindowSize();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);

	if (window == nullptr)
	{
		std::cerr << "Failed to create a window." << std::endl;
		glfwTerminate();
		glfwDestroyWindow(window);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	
}

Window::~Window()
{
	glfwTerminate();
	glfwDestroyWindow(window);
}

void Window::DetermineWindowSize()
{
	std::array<int, 3> inputInvalid = { true, true, true }; //res, width, height
	std::string space = "   ";
	std::string input;

	GLFWmonitor* monitorPrimary = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitorPrimary);
	std::cout << "Detected monitor resolution: " << videoMode->width << " x " << videoMode->height << std::endl << "\n";

	while (inputInvalid[0])
	{
		std::cout << "1) Run using the detected screen resolution? (y/n): ";
		std::getline(std::cin, input);

		constexpr int inputSize = 4;
		std::array<std::string, inputSize> arr = { "y", "Y", "n", "N" };
		if (std::find(arr.begin(), arr.end(), input) == arr.end())
		{
			std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
			continue;
		}
		else
			inputInvalid[0] = !inputInvalid[0];
	}

	if (input == "n" || input == "N")
	{
		while (inputInvalid[1])
		{
			std::cout << "\n" << "2) Please specify your preferred window resolution: " << "\n" << std::endl;
			std::cout << space << "2.1) Window width: ";
			std::getline(std::cin, input);

			if (std::find_if(input.begin(), input.end(), [](const char& c) {return isdigit(c); }) == input.end())
			{
				std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
				continue;
			}
			else if (std::stoi(input) > videoMode->width)
			{
				std::cerr << "->Error: Window width cannot be larger than the detected screen width. Try again." << std::endl;
				continue;
			}
			windowWidth = std::stoi(input);
			inputInvalid[1] = !inputInvalid[1];
		}

		while (inputInvalid[2])
		{
			std::cout << space << "2.2) Window height: ";
			std::getline(std::cin, input);

			if (std::find_if(input.begin(), input.end(), [](const char& c) {return isdigit(c); }) == input.end())
			{
				std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
				continue;
			}
			else if (std::stoi(input) > videoMode->width)
			{
				std::cerr << "->Error: Window height cannot be larger than the detected screen height. Try again." << std::endl;
				continue;
			}
			windowHeight = std::stoi(input);
			inputInvalid[2] = !inputInvalid[2];
		}
	}
	else
	{
		windowWidth = videoMode->width;
		windowHeight = videoMode->height;
	}
}

void Window::ClearScreen()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLFWwindow*& Window::GetWindow()
{
	return window;
}

const std::string& Window::GetGLSLversion()
{
	return glsl_version;
}
