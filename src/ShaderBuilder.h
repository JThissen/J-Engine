#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include "Shader.h"

class ShaderBuilder
{
private:
	std::unique_ptr<Shader> shader;
public:

	ShaderBuilder& create_shader();
	ShaderBuilder& attach_vertex_shader(const std::string& shaderPath);
	ShaderBuilder& attach_tess_control_shader(const std::string& shaderPath);
	ShaderBuilder& attach_tess_eval_shader(const std::string& shaderPath);
	ShaderBuilder& attach_geometry_shader(const std::string& shaderPath);
	ShaderBuilder& attach_fragment_shader(const std::string& shaderPath);
	Shader build_shader();
};

#endif