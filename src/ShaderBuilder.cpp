#include "ShaderBuilder.h"

ShaderBuilder& ShaderBuilder::create_shader()
{
	shader = std::make_unique<Shader>();
	return *this;
}

ShaderBuilder& ShaderBuilder::attach_vertex_shader(const std::string& shaderPath)
{
	shader->attach_shader(shaderPath, Shader::ShaderType::VERTEX);
	return *this;
}

ShaderBuilder& ShaderBuilder::attach_tess_control_shader(const std::string& shaderPath)
{
	shader->attach_shader(shaderPath, Shader::ShaderType::TESS_CONTROL);
	return *this;
}

ShaderBuilder& ShaderBuilder::attach_tess_eval_shader(const std::string& shaderPath)
{
	shader->attach_shader(shaderPath, Shader::ShaderType::TESS_EVAL);
	return *this;
}

ShaderBuilder& ShaderBuilder::attach_geometry_shader(const std::string& shaderPath)
{
	shader->attach_shader(shaderPath, Shader::ShaderType::GEOMETRY);
	return *this;
}

ShaderBuilder& ShaderBuilder::attach_fragment_shader(const std::string& shaderPath)
{
	shader->attach_shader(shaderPath, Shader::ShaderType::FRAGMENT);
	return *this;
}

Shader ShaderBuilder::build_shader()
{
	shader->builder_shader();
	return *shader.get();
}