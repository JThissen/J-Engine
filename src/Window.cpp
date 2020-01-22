#include "Window.h"


Window::Window(const std::string& window_name, const std::string& glsl_version)
	: window_name(window_name), glsl_version(glsl_version)
{
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW. Press any key to exit.");

	determine_window_size();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(window_width, window_height, window_name.data(), nullptr, nullptr);

	if (window == nullptr)
	{
		std::cerr << "Failed to create a window." << std::endl;
		glfwTerminate();
		glfwDestroyWindow(window);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::determine_window_size()
{
	std::unordered_map<std::string, bool> valid_input = { {"resolution", true}, {"width", true}, {"height", true} };
	std::array<std::string, 8> options = { "y", "n", "yes", "no"};
	std::string tab = "   ";
	std::string input;

	GLFWmonitor* monitorPrimary = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(monitorPrimary);

	std::cout << "Detected monitor resolution: " << videoMode->width << " x " << videoMode->height << std::endl << "\n";

	while (valid_input["resolution"])
	{
		std::cout << "1) Run using the detected screen resolution? (y/n): ";
		std::getline(std::cin, input);
		Helpers::Utility::to_lower_case(input);

		if (std::find(options.begin(), options.end(), input) != options.end())
			valid_input["resolution"] = !valid_input["resolution"];
		else
			std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
	}

	if (input == "n" || input == "no")
	{
		while (valid_input["width"])
		{
			std::cout << "\n" << "2) Please specify the desired window resolution: " << "\n" << std::endl;
			std::cout << tab << "2.1) Window width: ";
			std::getline(std::cin, input);

			if (!Helpers::Utility::all_digits(input))
			{
				std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
				continue;
			}

			if (std::stoi(input) > videoMode->width)
			{
				std::cerr << "->Error: Window width cannot be larger than the detected screen width. Try again." << std::endl;
				continue;
			}

			window_width = std::stoi(input);
			valid_input["width"] = !valid_input["width"];
		}

		while (valid_input["height"])
		{
			std::cout << tab << "2.2) Window height: ";
			std::getline(std::cin, input);

			if (!Helpers::Utility::all_digits(input))
			{
				std::cerr << "->Error: Invalid input. Try again." << "\n" << std::endl;
				continue;
			}

			if (std::stoi(input) > videoMode->height)
			{
				std::cerr << "->Error: Window height cannot be larger than the detected screen width. Try again." << std::endl;
				continue;
			}

			window_height = std::stoi(input);
			valid_input["height"] = !valid_input["height"];
		}
	}
	else
	{
		window_width = videoMode->width;
		window_height = videoMode->height;
	}
}

void Window::clear_screen()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLFWwindow* Window::get_window() const { return window; }
int Window::get_window_width() const { return window_width; }
int Window::get_window_height() const { return window_height; }
const std::string& Window::get_window_name() const { return window_name; }
const std::string& Window::get_glsl_version() const { return glsl_version; }