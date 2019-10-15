#ifndef SCENE_H
#define SCENE_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <array>
#include <vector>
#include <string>
#include <iostream>

#include "Camera.h"
#include "Bloom.h"
#include "VolumeLight.h"
#include "Terrain.h"

class Scene
{
private:

	Bloom				bloom;
	VolumeLight			volumeLight;
	Terrain				terrain;
	int					windowWidth;
	int					windowHeight;
	int					heightMapWidth;
	int					heightMapHeight;
	unsigned int		FBO;
	unsigned int		colorBuffer;
	unsigned int		VAO;
	unsigned int		VBO;
	unsigned int		EBO;
	std::vector<float>	heightMapVertexData;
	std::vector<short>	heightMapIndexData;

public:
	enum class Scenes { TERRAIN, VOLUMETRIC_LIGHT };

	Scene() = delete;
	Scene(int windowWidth, int windowHeight);
	~Scene();
	void DrawScene(float deltaTime, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, Camera& camera, Scenes scenes);
};

#endif