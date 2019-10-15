#ifndef VOLUMELIGHT_H
#define VOLUMELIGHT_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <string>

#include "ShaderBuilder.h"

class VolumeLight
{
private:
	unsigned int	colorBuffer[2];
	unsigned int	depthBuffer[2];
	unsigned int	frameBuffers[2];
	unsigned int	quadVAO;
	unsigned int	quadVBO;
	int				windowWidth;
	int				windowHeight;
	int				samples;
	float			exposure;
	float			decay;
	float			density;
	float			weight;
	float			bounds;
	glm::vec3		direction;
	glm::vec3		lightPosition;
	std::string		vs_volumelight;
	std::string		fs_volumelight;
	std::string		vs_occlusion;
	std::string		fs_occlusion;
	std::string		vs_normal;
	std::string		fs_normal;
	std::string		vs_light;
	std::string		fs_light;
	ShaderBuilder	shaderBuilder;
	Shader			shader_volumelight;
	Shader			shader_occlusion;
	Shader			shader_normal;
	Shader			shader_light;
	
public:
	VolumeLight();
	VolumeLight(const int windowWidth, const int windowHeight, int samples = 100, float exposure = 0.038f, float decay = 1.0f, 
		float density = 0.962f, float weight = 0.476f, float bounds = 5.0f, glm::vec3 lightPosition = glm::vec3(-1.0f, 0.0f, -4.0f));
	void DrawOcclusion(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
	void DrawNormal(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
	void DrawLightShafts(glm::mat4 projection, glm::mat4 view);
	void CalcMovement(float deltaTime, float velocity = 4.0f);
	unsigned int getColorBuffer(int value);
	int& getSamples();
	float& getExposure();
	float& getDecay();
	float& getDensity();
	float& getWeight();
};

#endif