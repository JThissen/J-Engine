#ifndef SHADER1_H
#define SHADER1_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ios>
#include <memory>
#include <array>
#include <unordered_map>
#include <vector>

class Shader
{
private:
	unsigned int				shader_program;
	std::vector<unsigned int>	current_attachments;

public:
	enum class ShaderType { VERTEX, TESS_CONTROL, TESS_EVAL, GEOMETRY, FRAGMENT };

	Shader();
	void use_shader();
	std::string read_shader(const std::string& shader_path);
	void attach_shader(const std::string& shader_path, ShaderType shader_type);
	void builder_shader();

	std::string get_shader_name(ShaderType shader_type) const;
	unsigned int get_shader_program() const;
};

#endif