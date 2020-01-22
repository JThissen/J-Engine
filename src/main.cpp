#include "Engine.h"

int main(int argc, char* argv[])
{
	std::string window_name = "J-Engine";
	std::string glsl_version = "#version 460";
	Engine& engine = Engine::get_instance(window_name, glsl_version);
	engine.initialize();
	return 0;
}