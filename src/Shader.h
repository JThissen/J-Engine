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
	unsigned int shaderProgram;
	std::vector<unsigned int> currentAttachments;

public:
	enum class ShaderType { VERTEX, TESS_CONTROL, TESS_EVAL, GEOMETRY, FRAGMENT };

	Shader();
	void UseShader();
	void ReadShader(const std::string& shaderPath, std::string& shaderCode);
	void AttachShader(const std::string& shaderPath, ShaderType shaderType);
	void BuildShader();
	std::string GetShaderName(ShaderType shaderType);
	unsigned int& GetShaderProgram();
};

#endif