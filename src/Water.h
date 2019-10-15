#ifndef WATER_H
#define WATER

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_access.hpp>
#include <string>
#include <vector>

#include "ShaderBuilder.h"

class Water
{
private:
	ShaderBuilder			shaderBuilder;
	Shader					shaderWater;
	unsigned int			VAOWater;
	unsigned int			VBOWater;

	int						windowWidth;
	int						windowHeight;
	std::vector<float>		vertexData;
	float					heightMin;
	float					heightMax;
	float					wavelength1;
	float					wavelength2;
	float					wavelength3;
	float					wavelength4;
	float					amplitude1;
	float					amplitude2;
	float					amplitude3;
	float					amplitude4;
	float					speed1;
	float					speed2;
	float					speed3;
	float					speed4;
	float					specularStrength;
	float					mixStrength;
	glm::vec2				dir1;
	glm::vec2				dir2;
	glm::vec2				dir3;
	glm::vec2				dir4;
	glm::vec3				shallowColor;
	glm::vec3				deepColor;
	bool					toggleGrid;
	float					lineWidth;

	unsigned int			texture_waterNormals;
	unsigned int			texture_vector2dmap;
	unsigned int			texture_reflectionColorBuffer;
	unsigned int			texture_refractionColorBuffer;
	unsigned int			texture_refractionDepthBuffer;

public:
	Water() = default;
	Water(int w, int h);
	void IntializeShaderParameters();
	void CreateShader();
	void CreateBuffers();
	void LoadStaticTextures();
	void RenderWater(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, unsigned int FBO, glm::vec3 tileSize, 
		int gridW, int gridH, int outerTessellation, int innerTessellation, glm::vec3 sunPosition, float specularShininess, 
		glm::vec3 ambientColor, float time, unsigned int& reflectionColorBuffer, unsigned int& refractionColorBuffer, unsigned int& refractionDepthBuffer);
};

#endif