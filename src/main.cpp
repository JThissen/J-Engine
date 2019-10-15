#include "Engine.h"

const std::string windowName = "J-Engine";
const std::string glslVersion = "#version 460";

int main(int argc, char* argv[])
{
	Engine& engine = Engine::CreateInstance(windowName, glslVersion);
	engine.Initialize();
	return 0;
}