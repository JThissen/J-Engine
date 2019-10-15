#include "SSAO.h"
#include "Helpers.h"

SSAO::SSAO(int w, int h) : windowWidth(w), windowHeight(h)
{
	IntializeShaderParameters();
	CreateSSAOShader();
	CreateSSAOBlurShader();
	CreateBuffers();
}

void SSAO::IntializeShaderParameters()
{
	SSAOSamples = 64;
	SSAONoiseTextureWidth = 4;
	kernelSize = 64;
	radius = 0.611f;
	bias = 0.012f;
	blurSSAO = true;
	showSSAO = false;
}

void SSAO::CreateSSAOShader()
{
	std::string vs_ssao = "./ShaderFiles/ssao.vs";
	std::string fs_ssao = "./ShaderFiles/ssao.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_ssao)
		.AttachFragmentShader(fs_ssao);

	shaderSSAO = shaderBuilder.BuildShader();
	shaderSSAO.UseShader();

	glUniform1i(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "colorBuffer"), 0);
	glUniform1i(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "gNormal"), 1);
	glUniform1i(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "gPosition"), 2);
	glUniform1i(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "texNoise"), 3);

	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;

	for (int i = 0; i < SSAOSamples; i++)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		float scale = static_cast<float>(i) / static_cast<float>(SSAOSamples); //closer to origin (x^2)
		sample *= Lerp<float>(0.1f, 1.0f, scale * scale);
		std::string item = "samples[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(shaderSSAO.GetShaderProgram(), item.c_str()), 1, glm::value_ptr(sample));
	}

	std::vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < (SSAONoiseTextureWidth * SSAONoiseTextureWidth); i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &SSAONoiseTexture);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SSAONoiseTextureWidth, SSAONoiseTextureWidth, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::CreateSSAOBlurShader()
{
	std::string vs_blur = "./ShaderFiles/blur2.vs";
	std::string fs_blur = "./ShaderFiles/blur2.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_blur)
		.AttachFragmentShader(fs_blur);

	shaderSSAOBlur = shaderBuilder.BuildShader();
	shaderSSAOBlur.UseShader();
	glUniform1i(glGetUniformLocation(shaderSSAOBlur.GetShaderProgram(), "ssaoInput"), 0);
}

void SSAO::CreateBuffers()
{
	glGenFramebuffers(1, &SSAOFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
	glGenTextures(1, &SSAOColorBuffer);
	glBindTexture(GL_TEXTURE_2D, SSAOColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOColorBuffer, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ssaFBO is incomplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &SSAOBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO);
	glGenTextures(1, &SSAOBlurColorBuffer);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOBlurColorBuffer, 0);
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ssaoBlurFBO is incomplete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SSAO::RenderSSAO(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const unsigned int& colorBuffer,
	const unsigned int& normalBuffer, const unsigned int& positionBuffer, unsigned int& ssaoColorBufferBlur, bool& displaySSAO)
{
	Helpers::Gui::ImGuiSSAO(windowWidth, windowHeight, kernelSize, radius, bias, blurSSAO, showSSAO);
	displaySSAO = showSSAO;

	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAO.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "kernelSize"), kernelSize);
	glUniform1f(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "radius"), radius);
	glUniform1f(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "bias"), bias);
	glUniform1f(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "windowWidth"), static_cast<float>(windowWidth));
	glUniform1f(glGetUniformLocation(shaderSSAO.GetShaderProgram(), "windowHeight"), static_cast<float>(windowHeight));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, positionBuffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);
	Helpers::Draw::drawQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAOBlur.UseShader();
	glUniform1i(glGetUniformLocation(shaderSSAOBlur.GetShaderProgram(), "blurSSAO"), blurSSAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, SSAOColorBuffer);
	Helpers::Draw::drawQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ssaoColorBufferBlur = SSAOBlurColorBuffer;
}

template<typename T>
float SSAO::Lerp(T a, T b, T c)
{
	return (b - a) * c + a;
}