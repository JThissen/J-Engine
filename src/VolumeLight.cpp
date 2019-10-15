#include "VolumeLight.h"
#include "Helpers.h"

VolumeLight::VolumeLight() {}

VolumeLight::VolumeLight(int windowWidth, int windowHeight, int samples, float exposure,
	float decay, float density, float weight, float bounds, glm::vec3 lightPosition)
	: windowWidth(windowWidth),
	windowHeight(windowHeight),
	samples(samples),
	exposure(exposure),
	decay(decay),
	density(density),
	weight(weight),
	bounds(bounds),
	lightPosition(lightPosition)
{
	direction = glm::vec3(1.0f, 0.0f, 0.0f);
	vs_volumelight = "./ShaderFiles/volumelight.vs";
	fs_volumelight = "./ShaderFiles/volumelight.fs";
	vs_occlusion = "./ShaderFiles/occlusion.vs";
	fs_occlusion = "./ShaderFiles/occlusion.fs";
	vs_normal = "./ShaderFiles/normal.vs";
	fs_normal = "./ShaderFiles/normal.fs";
	vs_light = "./ShaderFiles/light.vs";
	fs_light = "./ShaderFiles/light.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_volumelight)
		.AttachFragmentShader(fs_volumelight);
	shader_volumelight = shaderBuilder.BuildShader();

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_occlusion)
		.AttachFragmentShader(fs_occlusion);
	shader_occlusion = shaderBuilder.BuildShader();

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_normal)
		.AttachFragmentShader(fs_normal);
	shader_normal = shaderBuilder.BuildShader();

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_light)
		.AttachFragmentShader(fs_light);
	shader_light = shaderBuilder.BuildShader();

	glGenFramebuffers(2, frameBuffers);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);

		glGenTextures(1, &colorBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer[i], 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &depthBuffer[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer[i]);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer is incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader_volumelight.UseShader();
	glUniform1i(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "tex1"), 0);
	glUniform1i(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "tex2"), 1);
}

void VolumeLight::CalcMovement(float deltaTime, float velocity)
{
	float distance = deltaTime * velocity;

	if (lightPosition.x >= bounds)
		direction = glm::vec3(-1.0f, 0.0f, 0.0f);
	else if (lightPosition.x <= -bounds)
		direction = glm::vec3(1.0f, 0.0f, 0.0f);

	lightPosition += distance * direction;
}

void VolumeLight::DrawOcclusion(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[0]);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_occlusion.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

	for (int i = 0; i < 2; i++)
	{
		model = glm::mat4(1.0f);

		if (i == 0)
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
		else if (i == 1)
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader_occlusion.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		Helpers::Draw::drawCube();
	}

	shader_light.UseShader();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPosition);
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	Helpers::Draw::drawCube();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumeLight::DrawNormal(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[1]);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[1]);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_normal.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

	for (int i = 0; i < 2; i++)
	{
		model = glm::mat4(1.0f);

		if (i == 0)
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
		else if (i == 1)
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		Helpers::Draw::drawCube();
	}

	shader_light.UseShader();
	model = glm::mat4(1.0f);
	model = glm::translate(model, lightPosition);
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader_light.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	Helpers::Draw::drawCube();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VolumeLight::DrawLightShafts(glm::mat4 projection, glm::mat4 view)
{
	shader_volumelight.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projectionMatrix = glm::mat4() * projection;
	glm::mat4 viewMatrix = glm::mat4() * view;
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glm::vec4 lightNDCPosition = viewProjectionMatrix * glm::vec4(lightPosition, 1);
	lightNDCPosition /= lightNDCPosition.w;
	glm::vec2 lightScreenPosition = glm::vec2(
		(lightNDCPosition.x + 1) * 0.5,
		(lightNDCPosition.y + 1) * 0.5
	);

	glUniform2fv(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "lightScreenPos"), 1, glm::value_ptr(lightScreenPosition));
	glUniform1i(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "samples"), samples);
	glUniform1f(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "decay"), decay);
	glUniform1f(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "density"), density);
	glUniform1f(glGetUniformLocation(shader_volumelight.GetShaderProgram(), "weight"), weight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_volumelight.UseShader();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, colorBuffer[1]);
	Helpers::Draw::drawQuad();
}

unsigned int VolumeLight::getColorBuffer(int value) { return colorBuffer[value]; }
int& VolumeLight::getSamples() { return samples; }
float& VolumeLight::getExposure() { return exposure; }
float& VolumeLight::getDecay() { return decay; }
float& VolumeLight::getDensity() { return density; }
float& VolumeLight::getWeight() { return weight; }
