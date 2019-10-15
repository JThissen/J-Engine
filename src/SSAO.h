#ifndef SSAO_H
#define SSAO_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <string>
#include <random>
#include <vector>

#include "ShaderBuilder.h"

class SSAO
{
private:
	ShaderBuilder		shaderBuilder;
	Shader				shaderSSAOBlur;
	Shader				shaderSSAO;
	int					SSAOSamples;
	int					SSAONoiseTextureWidth;
	int					windowWidth;
	int					windowHeight;
	unsigned int		SSAONoiseTexture;
	unsigned int		SSAOFBO;
	unsigned int		SSAOColorBuffer;
	unsigned int		SSAOBlurFBO;
	unsigned int		SSAOBlurColorBuffer;

	int					kernelSize;
	float				radius;
	float				bias;
	bool				blurSSAO;
	bool				showSSAO;

public:
	SSAO() = default;
	SSAO(int w, int h);
	void IntializeShaderParameters();
	void CreateSSAOShader();
	void CreateSSAOBlurShader();
	void CreateBuffers();
	void RenderSSAO(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, const unsigned int& colorBuffer,
		const unsigned int& normalBuffer, const unsigned int& positionBuffer, unsigned int& ssaoColorBufferBlur, bool& displaySSAO);

	template<typename T>
	float Lerp(T a, T b, T c);
};

#endif