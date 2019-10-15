#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <random>

#include "Shader.h"
#include "Skybox.h"
#include "Camera.h"
#include "ShaderBuilder.h"
#include "Water.h"
#include "SSAO.h"

class Terrain
{
private:
	unsigned int		windowWidth;
	unsigned int		windowHeight;
	unsigned int		FBO;
	unsigned int		colorBuffer;
	unsigned int		positionBuffer;
	unsigned int		normalBuffer;
	unsigned int		depthBuffer;
	unsigned int		VAO;
	unsigned int		VBO;
	unsigned int		ssaoColorBufferBlur;
	unsigned int		reflectionFBO;
	unsigned int		refractionFBO;
	unsigned int		refractionDepthBuffer;
	unsigned int		reflectionColorBuffer;
	unsigned int		refractionColorBuffer;
	unsigned int		grassTexture;
	unsigned int		cliffTexture;
	unsigned int		snowTexture;
	unsigned int		sandTexture;
	unsigned int		heightMapTexture;
	unsigned int		colorBufferWater;
	unsigned int		texture_cubemap;
	unsigned int		shadowDepthFBO;
	unsigned int		shadowDepthBuffer;
	unsigned int		occlusionBuffer;
	int					innerTessellation;
	int					outerTessellation;
	int					innerTessellationWater;
	int					outerTessellationWater;
	int					reflrefrTessellation;
	int					gridW;
	int					gridH;
	int					noiseOctaves;
	int					noiseSize;
	int					noiseTexture;
	int					noiseTexture3D;
	int					instances;
	int					mode;
	int					shadowSamples;
	float				noiseFreq;
	float				heightScale;
	float				specularShininess;
	float				wrappedDiffuse;
	float				fogExp;
	float				de;
	float				di;
	float				fogUpperBound;
	float				fogLowerBound;
	float				lineWidth;
	float				minDepth;
	float				maxDepth;
	float				minTessellation;
	float				maxTessellation;
	float				time;
	float				waterHeight;
	float				specularStrength;
	float				mixStrength;
	float				snowBreakpoint;
	float				shadowBias;
	float				shadowAlpha;
	bool				toggleGrid;
	bool				cull;
	bool				cameraLOD;
	bool				showSSAO;
	ShaderBuilder		shaderBuilder;
	Shader				shader_terrain;
	Shader				shader_lighting;
	Shader				shader_water;
	Shader				shader_terrainEmpty;
	Shader				shader1_terrain;
	Shader				shader_normal;
	Water				water;
	SSAO				ssao;
	glm::vec2			translate;
	glm::vec3			tileSize;
	glm::vec3			ambientColor;
	glm::vec3			lightColor;
	glm::vec3			fogColor;
	glm::vec3			sunPosition;
	glm::vec3			shallowColor;
	glm::vec3			deepColor;
	glm::vec3			skyColor;
	glm::vec3			lightPos;
	glm::mat4			lightProjection;
	glm::mat4			lightView;
	std::vector<float>	vertexData;
	std::vector<short>	indexData;

	int samples;
	float exposure;
	float decay;
	float density;
	float weight;

	glm::vec3 volumeLightColor;

public:
	Terrain() = default;
	Terrain(int w, int h);
	void InitializeParameters();
	void CreateShaders();
	void CreateBuffers();
	void Draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera);
	unsigned int loadTerrainTexture(std::string path);
	void RenderTerrain(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera, Shader& shader, 
		unsigned int fbo, bool reflectionRefraction, bool useEmpty);
	void RenderPostProcessing(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);
	void RenderSun(const glm::mat4& projection, const glm::mat4& view);
};

#endif