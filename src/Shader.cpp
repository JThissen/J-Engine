#include "Shader.h"

Shader::Shader()
{
	shaderProgram = glCreateProgram();
}

void Shader::UseShader()
{
	glUseProgram(shaderProgram);
}

void Shader::ReadShader(const std::string& shaderPath, std::string& shaderCode)
{
	try
	{
		std::ifstream ifstream;
		std::stringstream ss;
		ifstream.open(shaderPath, std::ios::in);
		ss << ifstream.rdbuf();
		ifstream.close();
		shaderCode = ss.str();
	}
	catch (const std::ios_base::failure& e)
	{
		std::cerr << "Unable to read shader." << "\n" << "Error: " << e.what() << "\n" << "Code: " << e.code() << std::endl;
	}
}

void Shader::AttachShader(const std::string& shaderPath, ShaderType shaderType)
{
	std::string shaderCode;
	ReadShader(shaderPath, shaderCode);
	std::unique_ptr<const char*> shaderCode_c = std::make_unique<const char*>(shaderCode.c_str());
	unsigned int shader;

	switch (shaderType)
	{
	case ShaderType::VERTEX: shader = glCreateShader(GL_VERTEX_SHADER); break;
	case ShaderType::TESS_CONTROL: shader = glCreateShader(GL_TESS_CONTROL_SHADER); break;
	case ShaderType::TESS_EVAL: shader = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
	case ShaderType::GEOMETRY: shader = glCreateShader(GL_GEOMETRY_SHADER); break;
	case ShaderType::FRAGMENT: shader = glCreateShader(GL_FRAGMENT_SHADER); break;
	}

	glShaderSource(shader, 1, shaderCode_c.get(), nullptr);
	glCompileShader(shader);

	int compileSuccess;
	constexpr int bufferSize = 1024;
	std::array<char, bufferSize> infoLog;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);

	if (!compileSuccess)
	{
		glGetShaderInfoLog(shader, bufferSize, nullptr, infoLog.data());
		std::cerr << "Failed to compile " << GetShaderName(shaderType) << " shader. " << "Log: " << infoLog.data() << std::endl;
	}

	glAttachShader(shaderProgram, shader);
	currentAttachments.push_back(shader);
}

void Shader::BuildShader()
{
	glLinkProgram(shaderProgram);

	for (int i = currentAttachments.size(); i > 0; --i)
	{
		glDeleteShader(currentAttachments[i - 1]);
		currentAttachments.pop_back();
	}
}

std::string Shader::GetShaderName(ShaderType shaderType)
{
	std::unordered_map<ShaderType, std::string> enumMap =
	{
		{ShaderType::VERTEX, "VERTEX"},
		{ShaderType::TESS_CONTROL, "TESS_CONTROL"},
		{ShaderType::TESS_EVAL, "TESS_EVAL"},
		{ShaderType::GEOMETRY, "GEOMETRY"},
		{ShaderType::FRAGMENT, "FRAGMENT"}
	};

	std::unordered_map<ShaderType, std::string>::iterator it = enumMap.find(shaderType);

	if (it == enumMap.end())
		std::cerr << "Invalid shader type." << std::endl;

	return it->second;
}

unsigned int& Shader::GetShaderProgram() { return shaderProgram; }