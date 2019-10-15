#include "Water.h"
#include "Helpers.h"

Water::Water(int w, int h) : windowWidth(w), windowHeight(h)
{
	IntializeShaderParameters();
	CreateShader();
	CreateBuffers();
	LoadStaticTextures();
}

void Water::IntializeShaderParameters()
{
	vertexData = { 0.0f, 0.0f, 0.0f, 1.0f };
	heightMin = -0.157f;
	heightMax = 0.392f;
	wavelength1 = 0.196f;
	wavelength2 = 0.118f;
	wavelength3 = 0.157f;
	wavelength4 = 0.196f;
	amplitude1 = 0.004f;
	amplitude2 = 0.010f;
	amplitude3 = 0.009f;
	amplitude4 = 0.009f;
	speed1 = -0.039f;
	speed2 = -0.039f;
	speed3 = 0.039f;
	speed4 = 0.039f;
	specularStrength = 0.2f;
	mixStrength = 0.018f;
	dir1 = glm::vec2(0.508, 0.529);
	dir2 = glm::vec2(0.729, -0.395);
	dir3 = glm::vec2(1.0, 0.765);
	dir4 = glm::vec2(-0.288, 1.0);
	shallowColor = glm::vec3(0.0f, 0.64f, 0.68f);
	deepColor = glm::vec3(0.02f, 0.05f, 0.10f);
	toggleGrid = false;
	lineWidth = 0.002f;
}

void Water::CreateShader()
{
	std::string vs_water = "./ShaderFiles/water.vs";
	std::string tesc_water = "./ShaderFiles/water.tesc";
	std::string tese_water = "./ShaderFiles/water.tese";
	std::string gs_water = "./ShaderFiles/water.gs";
	std::string fs_water = "./ShaderFiles/water.fs";

	shaderBuilder.CreateShader()
		.AttachVertexShader(vs_water)
		.AttachTessControlShader(tesc_water)
		.AttachTessEvaShader(tese_water)
		.AttachGeometryShader(gs_water)
		.AttachFragmentShader(fs_water);

	shaderWater = shaderBuilder.BuildShader();
	shaderWater.UseShader();

	glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "waterNormals"), 0);
	glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "reflection"), 1);
	glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "refraction"), 2);
	glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "vector2dmap"), 3);
	glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "refractionDepthBuffer"), 4);
}

void Water::CreateBuffers()
{
	glGenVertexArrays(1, &VAOWater);
	glBindVertexArray(VAOWater);
	glGenBuffers(1, &VBOWater);
	glBindBuffer(GL_ARRAY_BUFFER, VBOWater);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	glBindVertexArray(0);
}

void Water::LoadStaticTextures()
{
	texture_waterNormals = Helpers::Load::loadTexture("./Images/textures/waterNormals.jpg");
	texture_vector2dmap = Helpers::Load::loadTexture("./Images/textures/2dvectormap.png");
}

void Water::RenderWater(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, unsigned int FBO,
	glm::vec3 tileSize, int gridW, int gridH, int outerTessellation, int innerTessellation, glm::vec3 sunPosition,
	float specularShininess, glm::vec3 ambientColor, float time, unsigned int& reflectionColorBuffer, 
	unsigned int& refractionColorBuffer, unsigned int& refractionDepthBuffer)
{
	Helpers::Gui::ImGuiWater(windowWidth, windowHeight, heightMin, heightMax, dir1, dir2, dir3, dir4, wavelength1, wavelength2, wavelength3, wavelength4, amplitude1,
		amplitude2, amplitude3, amplitude4, speed1, speed2, speed3, speed4, innerTessellation, outerTessellation,
		specularStrength, mixStrength, shallowColor, deepColor, toggleGrid, lineWidth);

	texture_reflectionColorBuffer = reflectionColorBuffer;
	texture_refractionColorBuffer = refractionColorBuffer;
	texture_refractionDepthBuffer = refractionDepthBuffer;
	shaderWater.UseShader();

	{
		glUniformMatrix4fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "toggleGrid"), toggleGrid);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "lineWidth"), lineWidth);
		glUniform3fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "tileSize"), 1, glm::value_ptr(tileSize));
		glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "gridW"), gridW);
		glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "gridH"), gridH);
		glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "outerTessellationWater"), outerTessellation);
		glUniform1i(glGetUniformLocation(shaderWater.GetShaderProgram(), "innerTessellationWater"), innerTessellation);
		glUniform3fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "sunPosition"), 1, glm::value_ptr(sunPosition));
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "shininess"), specularShininess);
		glUniform3fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "ambientColor"), 1, glm::value_ptr(ambientColor));
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "time"), time);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "heightMin"), heightMin);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "heightMax"), heightMax);
		glUniform2fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "dir1"), 1, glm::value_ptr(dir1));
		glUniform2fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "dir2"), 1, glm::value_ptr(dir2));
		glUniform2fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "dir3"), 1, glm::value_ptr(dir3));
		glUniform2fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "dir4"), 1, glm::value_ptr(dir4));
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "wavelength1"), wavelength1);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "wavelength2"), wavelength2);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "wavelength3"), wavelength3);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "wavelength4"), wavelength4);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "amplitude1"), amplitude1);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "amplitude2"), amplitude2);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "amplitude3"), amplitude3);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "amplitude4"), amplitude4);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "speed1"), speed1);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "speed2"), speed2);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "speed3"), speed3);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "speed4"), speed4);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "specularStrength"), specularStrength);
		glUniform1f(glGetUniformLocation(shaderWater.GetShaderProgram(), "mixStrength"), mixStrength);
		glUniform3fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "shallowColor"), 1, glm::value_ptr(shallowColor));
		glUniform3fv(glGetUniformLocation(shaderWater.GetShaderProgram(), "deepColor"), 1, glm::value_ptr(deepColor));
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture_waterNormals);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texture_reflectionColorBuffer);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texture_refractionColorBuffer);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, texture_vector2dmap);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, texture_refractionDepthBuffer);
	glBindVertexArray(VAOWater);
	glDrawArraysInstanced(GL_PATCHES, 0, 1, gridW * gridH);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
}