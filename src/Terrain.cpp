#include "Terrain.h"
#include "Helpers.h"
#include "stb_image.h"
#include "Model.h"
#include "Mesh.h"

Terrain::Terrain(int w, int h) : windowWidth(w), windowHeight(h)
{
	InitializeParameters();
	CreateShaders();
	CreateBuffers();
}

void Terrain::InitializeParameters()
{
	water = Water(windowWidth, windowHeight);
	ssao = SSAO(windowWidth, windowHeight);
	vertexData = { 0.0f, 0.0f, 0.0f, 1.0f };
	innerTessellation = outerTessellation = 32;
	innerTessellationWater = outerTessellationWater = 10;
	gridW = gridH = 16;
	instances = gridW * gridH;
	noiseFreq = 0.25f;
	noiseOctaves = 8;
	translate = glm::vec2(0.0f, 0.0f);
	heightScale = 3.5f;
	noiseSize = 300;
	tileSize = glm::vec3(1.0f, 0.0f, 1.0f);
	specularShininess = 100.0f;
	ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColor = glm::vec3(1.5f, 1.5f, 1.5f);
	wrappedDiffuse = 0.9f;
	fogExp = 0.1f;
	de = 0.068f;
	di = 0.043f;
	fogUpperBound = 1.668f;
	fogLowerBound = 0.4f;
	fogColor = glm::vec3(0.9f, 0.9f, 0.9f);
	lineWidth = 0.001f;
	toggleGrid = false;
	cull = true;
	cameraLOD = false;
	minDepth = 0.0f;
	maxDepth = 25.0f;
	minTessellation = 12.0f;
	maxTessellation = 32.0f;
	sunPosition = glm::normalize(glm::vec3(-1.0, -0.25, 1.0));
	waterHeight = 0.1f;
	reflrefrTessellation = 5;
	snowBreakpoint = 1.0f;
	skyColor = glm::vec3(0.7f, 0.8f, 1.0f) * 0.7f;
	lightPos = glm::vec3(43.0f, 11.0f, -31.0f);
	shadowBias = 0.001f;
	shadowAlpha = 0.15f;
	shadowSamples = 7;
	samples = 100;
	exposure = 0.038f;
	decay = 1.0f;
	density = 0.962f;
	weight = 0.476f;
	volumeLightColor = glm::vec3(1.0, 1.0, 0.8);
}

void Terrain::CreateShaders()
{
	std::string vs_terrain = "./ShaderFiles/terrain.vs";
	std::string tesc_terrain = "./ShaderFiles/terrain.tesc";
	std::string tese_terrain = "./ShaderFiles/terrain.tese";
	std::string gs_terrain = "./ShaderFiles/terrain.gs";
	std::string fs_terrain = "./ShaderFiles/terrain.fs";
	std::string fs_terrainEmpty = "./ShaderFiles/terrainEmpty.fs";
	std::string vs_shadowDepth = "./ShaderFiles/shadowDepth.vs";
	std::string fs_shadowDepth = "./ShaderFiles/shadowDepth.fs";
	std::string vs_lighting = "./ShaderFiles/lighting.vs";
	std::string fs_lighting = "./ShaderFiles/lighting.fs";
	std::string vs_normal = "./ShaderFiles/normal.vs";
	std::string fs_normal = "./ShaderFiles/normal.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_normal)
		.AttachFragmentShader(fs_normal);

	shader_normal = shaderBuilder.BuildShader();

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_terrain)
		.AttachTessControlShader(tesc_terrain)
		.AttachTessEvaShader(tese_terrain)
		.AttachGeometryShader(gs_terrain)
		.AttachFragmentShader(fs_terrain);

	shader_terrain = shaderBuilder.BuildShader();
	shader_terrain.UseShader();
	noiseTexture = Helpers::Noise::createNoiseTexture2D(GL_R16F, true);
	grassTexture = Helpers::Load::loadTexture("./Images/textures/grass.jpg");
	snowTexture = Helpers::Load::loadTexture("./Images/textures/snow.jpg");
	sandTexture = Helpers::Load::loadTexture("./Images/textures/brownish.jpg");
	cliffTexture = Helpers::Load::loadTexture("./Images/textures/cliff.jpg");
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "randTex"), 0);
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "grassSampler"), 1);
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "snowSampler"), 2);
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "sandSampler"), 3);
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "cliffSampler"), 4);
	glUniform1i(glGetUniformLocation(shader_terrain.GetShaderProgram(), "shadowDepthBuffer"), 5);

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_terrain)
		.AttachTessControlShader(tesc_terrain)
		.AttachTessEvaShader(tese_terrain)
		.AttachGeometryShader(gs_terrain)
		.AttachFragmentShader(fs_terrainEmpty);

	shader_terrainEmpty = shaderBuilder.BuildShader();
	shader_terrainEmpty.UseShader();
	glUniform1i(glGetUniformLocation(shader_terrainEmpty.GetShaderProgram(), "randTex"), 0);

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_lighting)
		.AttachFragmentShader(fs_lighting);

	shader_lighting = shaderBuilder.BuildShader();
	shader_lighting.UseShader();
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "gAlbedo"), 0);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "ssao"), 1);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "depthBuffer"), 2);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "gPosition"), 3);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "occlusionBuffer"), 4);
}

void Terrain::Draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera)
{
	Helpers::Gui::ImGuiTessellationWindow(windowWidth, windowHeight, gridW, gridH, tileSize, outerTessellation, innerTessellation, noiseFreq,
		noiseOctaves, translate, heightScale, noiseSize, specularShininess, ambientColor, wrappedDiffuse, fogExp,
		de, di, fogUpperBound, fogLowerBound, fogColor, toggleGrid, lineWidth, cull, cameraLOD, minDepth, maxDepth,
		minTessellation, maxTessellation, lightColor, sunPosition, snowBreakpoint, lightPos, shadowBias, shadowAlpha, 
		shadowSamples, samples, exposure, decay, density, weight, volumeLightColor);

	instances = gridW * gridH;
	time = static_cast<float>(glfwGetTime()) * 0.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	RenderTerrain(lightProjection, lightView, model, camera, shader_terrainEmpty, shadowDepthFBO, false, true);
	RenderTerrain(projection, view, model, camera, shader_terrain, FBO, true, false);

	water.RenderWater(projection, view, model, FBO, tileSize, gridW, gridH, innerTessellationWater, outerTessellationWater,
		sunPosition, specularShininess, ambientColor, time, reflectionColorBuffer, refractionColorBuffer, refractionDepthBuffer);
	
	ssao.RenderSSAO(projection, view, model, colorBuffer, normalBuffer, positionBuffer, ssaoColorBufferBlur, showSSAO);

	RenderSun(projection, view);
	RenderPostProcessing(projection, view, model);
	Helpers::Gui::ImGuiRender();
}

unsigned int Terrain::loadTerrainTexture(std::string path)
{
	unsigned int texture;
	int width;
	int height;
	int nrChannels;

	unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

	if (image != nullptr)
	{
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cerr << "Failed to load terrain texture." << std::endl;

	stbi_image_free(image);
	return texture;
}

void Terrain::CreateBuffers()
{
	//shadow mapping buffer
	const unsigned int shadowTextureWidth = 1024;
	const unsigned int shadowTextureHeight = 1024;

	glGenFramebuffers(1, &shadowDepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
	glGenTextures(1, &shadowDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, shadowDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "shadowDepthFBO is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//reflection buffer
	glGenFramebuffers(1, &reflectionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
	glGenTextures(1, &reflectionColorBuffer);
	glBindTexture(GL_TEXTURE_2D, reflectionColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionColorBuffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "reflectionFBO is incomplete!" << std::endl;

	unsigned int attachmentsReflection[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachmentsReflection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//refraction buffer
	glGenFramebuffers(1, &refractionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glGenTextures(1, &refractionColorBuffer);
	glBindTexture(GL_TEXTURE_2D, refractionColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionColorBuffer, 0);

	glGenTextures(1, &refractionDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, refractionDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthBuffer, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "refractionFBO is incomplete!" << std::endl;

	unsigned int attachmentsRefracion[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachmentsRefracion);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//terrain buffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glGenTextures(1, &normalBuffer);
	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer, 0);

	glGenTextures(1, &positionBuffer);
	glBindTexture(GL_TEXTURE_2D, positionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionBuffer, 0);

	glGenTextures(1, &colorBufferWater);
	glBindTexture(GL_TEXTURE_2D, colorBufferWater);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, colorBufferWater, 0);

	glGenTextures(1, &occlusionBuffer);
	glBindTexture(GL_TEXTURE_2D, occlusionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, occlusionBuffer, 0);

	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader_terrain.UseShader();
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(0);
}

void Terrain::RenderTerrain(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera, Shader& shader,
	unsigned int fbo, bool reflectionRefraction, bool useEmpty)
{
	shader.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
	glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
	glUniform3fv(glGetUniformLocation(shader.GetShaderProgram(), "tileSize"), 1, glm::value_ptr(tileSize));
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "gridW"), gridW);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "gridH"), gridH);

	if (useEmpty)
	{
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "outerTessFactor"), reflrefrTessellation);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "innerTessFactor"), reflrefrTessellation);
	}
	else
	{
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "outerTessFactor"), outerTessellation);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "innerTessFactor"), innerTessellation);
	}

	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "noiseFreq"), noiseFreq);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "noiseOctaves"), noiseOctaves);
	glUniform2fv(glGetUniformLocation(shader.GetShaderProgram(), "translate"), 1, glm::value_ptr(translate));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "heightScale"), heightScale);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "noiseSize"), noiseSize);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "cameraLOD"), cameraLOD);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "minDepth"), minDepth);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "maxDepth"), maxDepth);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "minTessellation"), minTessellation);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "maxTessellation"), maxTessellation);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "snowBreakpoint"), snowBreakpoint);
	glUniform3fv(glGetUniformLocation(shader.GetShaderProgram(), "sunPosition"), 1, glm::value_ptr(lightPos));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "shadowBias"), shadowBias);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "shadowAlpha"), shadowAlpha);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "shadowSamples"), static_cast<float>(shadowSamples));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "shininess"), specularShininess);
	glUniform3fv(glGetUniformLocation(shader.GetShaderProgram(), "ambientColor"), 1, glm::value_ptr(ambientColor));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "fogExp"), fogExp);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "de"), de);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "di"), di);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "fogUpperBound"), fogUpperBound);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "fogLowerBound"), fogLowerBound);
	glUniform3fv(glGetUniformLocation(shader.GetShaderProgram(), "fogColor"), 1, glm::value_ptr(fogColor));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "toggleGrid"), toggleGrid);
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "lineWidth"), lineWidth);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "cull"), cull);
	glUniform3fv(glGetUniformLocation(shader.GetShaderProgram(), "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform1f(glGetUniformLocation(shader.GetShaderProgram(), "diffuseAmplitude"), wrappedDiffuse);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "mode"), mode);

	if (reflectionRefraction)
	{
		//render reflection texture
		glEnable(GL_CLIP_DISTANCE0);
		mode = 1;
		float distance = (camera.GetCameraPos().y - waterHeight) * 2.0f;
		camera.SetCameraPos(camera.GetCameraPos() - distance);
		camera.InvertPitch();
		glUniformMatrix4fv(glGetUniformLocation(shader.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "mode"), mode);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "outerTessFactor"), reflrefrTessellation);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "innerTessFactor"), reflrefrTessellation);
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sandTexture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliffTexture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadowDepthBuffer);
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		camera.SetCameraPos(camera.GetCameraPos() + distance);
		camera.InvertPitch();

		//render refraction texture
		mode = 2;
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "mode"), mode);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "outerTessFactor"), outerTessellation);
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "innerTessFactor"), innerTessellation);
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sandTexture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliffTexture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadowDepthBuffer);
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_CLIP_DISTANCE0);
	}

	//render terrain
	mode = 0;
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "mode"), mode);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "outerTessFactor"), outerTessellation);
	glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), "innerTessFactor"), innerTessellation);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	if (!useEmpty)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, sandTexture);
		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, cliffTexture);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, shadowDepthBuffer);
	}

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_PATCHES, 0, 1, instances);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::RenderPostProcessing(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader_lighting.UseShader();
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
	glUniformMatrix4fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
	glUniform3fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "sunPosition"), 1, glm::value_ptr(sunPosition));
	glUniform3fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "lightPosition"), 1, glm::value_ptr(lightPos));
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "diffuseAmplitude"), wrappedDiffuse);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "de"), de);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "di"), di);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "fogUpperBound"), fogUpperBound);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "fogLowerBound"), fogLowerBound);
	glUniform3fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "fogColor"), 1, glm::value_ptr(fogColor));
	glUniform3fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "ambientColor"), 1, glm::value_ptr(ambientColor));
	glUniform3fv(glGetUniformLocation(shader_lighting.GetShaderProgram(), "skyColor"), 1, glm::value_ptr(skyColor));
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "shininess"), specularShininess);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "showSSAO"), showSSAO);
	glUniform1i(glGetUniformLocation(shader_lighting.GetShaderProgram(), "samples"), samples);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "decay"), decay);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "density"), density);
	glUniform1f(glGetUniformLocation(shader_lighting.GetShaderProgram(), "weight"), weight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, positionBuffer);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, occlusionBuffer);
	Helpers::Draw::drawQuad();
}

void Terrain::RenderSun(const glm::mat4& projection, const glm::mat4& view)
{
	shader_normal.UseShader();
	std::string path = "./Images/sphere/sphere.obj";
	Model model(path);

	glm::mat4 m = glm::mat4(1.0f);
	m = glm::translate(m, lightPos);
	m = glm::scale(m, glm::vec3(0.5f));

	glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform3fv(glGetUniformLocation(shader_normal.GetShaderProgram(), "lightColor"), 1, glm::value_ptr(volumeLightColor));
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	model.Draw(shader_normal);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}