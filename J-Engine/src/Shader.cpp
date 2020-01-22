#include "Shader.h"

Shader::Shader()
{
	shader_program = glCreateProgram();
}

void Shader::use_shader()
{
	glUseProgram(shader_program);
}

std::string Shader::read_shader(const std::string& shader_path)
{
	std::fstream fs;
	std::string buffer;
	std::vector<char> writable_buffer;
	fs.open(shader_path, std::ios::in | std::ios::beg);

	try
	{
		if (fs.is_open())
		{
			fs.seekg(0, std::ios::end);
			buffer.resize(fs.tellg());
			writable_buffer.resize(buffer.size());
			fs.seekg(0, std::ios::beg);
			fs.read(writable_buffer.data(), buffer.size());
			fs.close();
		}
		else
			std::cout << "Could not read shader with path: " << shader_path.data() << std::endl;
	}
	catch (const std::ios_base::failure & e)
	{
		std::cerr << "Unable to read shader." << "\n" << "Error: " << e.what() << "\n" << "Code: " << e.code() << std::endl;
	}

	return std::string(writable_buffer.begin(), writable_buffer.end());
}

void Shader::attach_shader(const std::string& shader_path, ShaderType shader_type)
{
	std::string shader_code = read_shader(shader_path);
	const char* shader_handle_c[] = { shader_code.data() };
	unsigned int shader;
	int compileSuccess;
	constexpr int bufferSize = 1024;
	std::array<char, bufferSize> infoLog;

	switch (shader_type)
	{
		case ShaderType::VERTEX: shader = glCreateShader(GL_VERTEX_SHADER); break;
		case ShaderType::TESS_CONTROL: shader = glCreateShader(GL_TESS_CONTROL_SHADER); break;
		case ShaderType::TESS_EVAL: shader = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
		case ShaderType::GEOMETRY: shader = glCreateShader(GL_GEOMETRY_SHADER); break;
		case ShaderType::FRAGMENT: shader = glCreateShader(GL_FRAGMENT_SHADER); break;
	}

	glShaderSource(shader, 1, shader_handle_c, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);

	if (!compileSuccess)
	{
		glGetShaderInfoLog(shader, bufferSize, nullptr, infoLog.data());
		std::cerr << "Failed to compile " << get_shader_name(shader_type) << " shader. " << "Log: " << infoLog.data() << std::endl;
	}

	glAttachShader(shader_program, shader);
	current_attachments.push_back(shader);
}

void Shader::builder_shader()
{
	glLinkProgram(shader_program);

	for (int i = current_attachments.size(); i > 0; --i)
	{
		glDeleteShader(current_attachments[i - 1]);
		current_attachments.pop_back();
	}
}

std::string Shader::get_shader_name(ShaderType shader_type) const
{
	std::unordered_map<ShaderType, std::string> enumMap =
	{
		{ShaderType::VERTEX, "VERTEX"},
		{ShaderType::TESS_CONTROL, "TESS_CONTROL"},
		{ShaderType::TESS_EVAL, "TESS_EVAL"},
		{ShaderType::GEOMETRY, "GEOMETRY"},
		{ShaderType::FRAGMENT, "FRAGMENT"}
	};

	std::unordered_map<ShaderType, std::string>::iterator it = enumMap.find(shader_type);

	if (it == enumMap.end())
		std::cerr << "Invalid shader type." << std::endl;

	return it->second;
}

unsigned int Shader::get_shader_program() const { return shader_program; }