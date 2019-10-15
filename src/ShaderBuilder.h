#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include "Shader.h"

class ShaderBuilder
{
private:
	std::unique_ptr<Shader> shader;
public:

	ShaderBuilder& CreateShader()
	{
		shader = std::make_unique<Shader>();
		return *this;
	}

	ShaderBuilder& AttachVertexShader(const std::string& shaderPath)
	{
		shader->AttachShader(shaderPath, Shader::ShaderType::VERTEX);
		return *this;
	}

	ShaderBuilder& AttachTessControlShader(const std::string& shaderPath)
	{
		shader->AttachShader(shaderPath, Shader::ShaderType::TESS_CONTROL);
		return *this;
	}

	ShaderBuilder& AttachTessEvaShader(const std::string& shaderPath)
	{
		shader->AttachShader(shaderPath, Shader::ShaderType::TESS_EVAL);
		return *this;
	}

	ShaderBuilder& AttachGeometryShader(const std::string& shaderPath)
	{
		shader->AttachShader(shaderPath, Shader::ShaderType::GEOMETRY);
		return *this;
	}

	ShaderBuilder& AttachFragmentShader(const std::string& shaderPath)
	{
		shader->AttachShader(shaderPath, Shader::ShaderType::FRAGMENT);
		return *this;
	}

	Shader BuildShader()
	{
		shader->BuildShader();
		return *shader.get();
	}
};

#endif